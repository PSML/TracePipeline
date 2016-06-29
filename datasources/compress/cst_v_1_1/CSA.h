/*
 * Compressed suffix array by Veli Mäkinen,
 * additional changes by Niko Välimäki
 */

#ifndef _CSA_H_
#define _CSA_H_
#include <iostream>
#include <queue>
#include <iomanip>
#include "BitRank.h"
#include "wtreebwt.h"

class CSA {
 private:
  class TCodeEntry {
  public:
    unsigned count;
    unsigned bits;
    unsigned code;
    TCodeEntry() { count=0; bits=0; code=0u; };
  };   
  
  class THuffAlphabetRank {
    // using fixed 0...255 alphabet
  private:
    BitRank *bitrank;
    THuffAlphabetRank *left;
    THuffAlphabetRank *right;
    TCodeEntry *codetable;
    alpha_t ch;
    bool leaf;
    
  public:
    THuffAlphabetRank(alpha_t *, trace_t, TCodeEntry *, unsigned);
    ~THuffAlphabetRank();
    bool Test(alpha_t *, trace_t);
        
    inline trace_t rank(alpha_t c, trace_t i)
    {
      // returns the number of characters c before and including position i
      THuffAlphabetRank *temp = this;
      if (codetable[c].count == 0) return 0;
      unsigned level = 0;
      unsigned code = codetable[c].code;
      
      //cout << "i=" << i << endl;
      while (!temp->leaf) {
	if ((code & (1u << level)) == 0) {
	  //cout << "bitrank->rank(" << i << ") = " << temp->bitrank->rank(i) << endl;
	  i = i-temp->bitrank->rank(i);
	  temp = temp->left;
	  //cout << "i=" << i << ", left" << endl;
	} else {
	  //cout << "bitrank->rank(" << i << ") = " << temp->bitrank->rank(i) << endl;
	  i = temp->bitrank->rank(i) - 1;
	  temp = temp->right;
	  //cout << "i=" << i << ", right" << endl;
	}
	++level;
      } 
      return i + 1;
    };
    
    inline bool IsCharAtPos(int c, trace_t i)
    {
      THuffAlphabetRank *temp = this;
      if (codetable[c].count == 0) return false;
      unsigned level = 0;
      unsigned code = codetable[c].code;      
      while (!temp->leaf) {
	if ((code & (1u<<level)) == 0) {
	  if (temp->bitrank->IsBitSet(i)) return false;
	  i = i-temp->bitrank->rank(i); 
	  temp = temp->left; 
	} else { 
	  if (!temp->bitrank->IsBitSet(i)) return false;         
	  i = temp->bitrank->rank(i)-1; 
	  temp = temp->right;
	}
	++level;
      } 
      return true;
    };
    
    inline int charAtPos(trace_t i) {
      THuffAlphabetRank *temp = this;
      while (!temp->leaf) {
	if (temp->bitrank->IsBitSet(i)) {
	  i = temp->bitrank->rank(i)-1;
	  temp = temp->right;
	}
	else {
	  i = i-temp->bitrank->rank(i); 
	  temp = temp->left;      
	}         
      }
      return (int)temp->ch;
    }
  };

  class node {
  private:
    unsigned weight;
    alpha_t value;
    node *child0;
    node *child1;
    
    void maketable( unsigned code, unsigned bits, TCodeEntry *codetable ) const;
    static void count_chars(alpha_t *, trace_t , TCodeEntry *);
    static unsigned SetBit(unsigned , unsigned , unsigned );
    
  public:
    node( alpha_t c = 0, unsigned i = 0 ) {
      value = c;
      weight = i;
      child0 = 0;
      child1 = 0;
    }
        
    node( node* c0, node *c1 ) {
      value = 0;
      weight = c0->weight + c1->weight;
      child0 = c0;
      child1 = c1;
    }

      
    bool operator>( const node &a ) const {
      return weight > a.weight;
    }

    static TCodeEntry *makecodetable(alpha_t *, trace_t);
  };
    
  static const unsigned char print = 1;
  static const unsigned char report = 1;
  trace_t n;
  unsigned samplerate;
  trace_t C[ALPHABETSIZE];
  trace_t bwtEndPos;
  THuffAlphabetRank *alphabetrank;
  BitRank *sampled; 
  trace_t *suffixes;
  trace_t *positions;
  TCodeEntry *codetable;
    
  // Private methods
  alpha_t * BWT(alpha_t *);
  alpha_t * LoadFromFile(const char *);
  void SaveToFile(const char *, alpha_t *);
  void maketables();

 public:
  CSA(alpha_t *, trace_t, unsigned, const char * = 0, const char * = 0);
  ~CSA();
  trace_t Search(alpha_t *, trace_t, trace_t *, trace_t *);
  trace_t lookup(trace_t);
  trace_t inverse(trace_t);
  trace_t Psi(trace_t);
  alpha_t * substring(trace_t, trace_t);
};

#endif
