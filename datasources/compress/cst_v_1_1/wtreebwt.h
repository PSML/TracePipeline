/***************************************************************************
 *   Copyright (C) 2006 by Wolfgang Gerlach   *
 *      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// ------ Dynamic Sequence with Indels -----
// uses huffman shaped wavelet tree
// space: O(n log \sigma) time: O(log n log \sigma)
// papers: V. Maekinen, G. Navarro. Dynamic Entropy-Compressed Sequences and Full-Text
//           Indexes. CPM 2006, Chapter 3.6 

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <bitset>

#include "bittree.h"

/*
// defined in Tools.h
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif
*/

using namespace std;

class WaveletNode {
 public:
	
  WaveletNode *left;
  WaveletNode *right;
  WaveletNode *parent;
  trace_t weight; // used only while construction
  alpha_t c0;     // used also while construction
  alpha_t c1;
  
  BVTree *bittree;
  
 WaveletNode(alpha_t c)
   : left(0), right(0), parent(0), weight(0), c0(c), bittree(0){}

 WaveletNode(WaveletNode *left, WaveletNode *right)
   : left(left), right(right), parent(0), bittree(0) {
    weight = left->weight + right->weight;
    left->parent = this;
    right->parent = this;
  }
	
  ~WaveletNode() {
    delete bittree;
  }
	
  bool operator>(const WaveletNode &a) const {
    return (weight > a.weight);
  }
};

namespace std
{
  template<> struct greater<WaveletNode*> {
    bool operator()(WaveletNode const* p1, WaveletNode const* p2)
    {
      if(!p1) return false;
      if(!p2) return true;
      return *p1 > *p2;
    }
  };
} 
 
class DynFMI {
 public:		
  //construct bwt
  DynFMI(alpha_t *text, trace_t n);		
  
  ~DynFMI();
		
  //get result (bwt)
  alpha_t* getBWT();
  
  trace_t getSize() { return root->bittree->getPositions(); }		
  	
  //LF(i)-mapping: C[L[i]]+rank_L[i](L,i)
  trace_t LFmapping(trace_t i) {
    alpha_t s=(*this)[i];
    return (trace_t)getNumberOfSymbolsSmallerThan(s) + rank(s,i);
  }
		
  void printDynFMIContent(ostream& stream);
		
 private:
  WaveletNode *root;
  WaveletNode **leaves;
		
  trace_t codes[ALPHABETSIZE];
  int codelengths[ALPHABETSIZE];
  trace_t C[ALPHABETSIZE+ALPHABETSIZE];
  
  trace_t iterate;
  		
  alpha_t operator[](trace_t i);
  void addText(alpha_t *str, trace_t n);

  trace_t rank(alpha_t c, trace_t i);
  trace_t select(alpha_t c, trace_t i);
	
  void insert(alpha_t c, trace_t i);

  // functions
  trace_t getNumberOfSymbolsSmallerThan(alpha_t c);
		
  void initEmptyDynFMI(alpha_t *text);
		
  void makeCodes(trace_t code, int bits, WaveletNode *node);
  void deleteLeaves(WaveletNode *node);
  void appendBVTrees(WaveletNode *node);
		
  void deleteDynFMINodes(WaveletNode *n);
		
  //Iterator (public??)
  void iterateReset();
  bool iterateNext();
  alpha_t iterateGetSymbol();
  void recursiveIterateResetWaveletNode(WaveletNode *w);	

  // small help functions
  double log2(double x) {
    return (log10(x) / log10((double)2));
  }
		
  int binaryTree_parent(int i) {
    return (int)floor((double)i/2);
  }

  int binaryTree_left(int i) {
    return 2*i;
  }

  int binaryTree_right(int i) {
    return 2*i + 1;
  }

  bool binaryTree_isLeftChild(int i) {
    return (i%2==(int)0);
  }

  bool binaryTree_isRightChild(int i) {
    return (i%2==(int)1);
  }		
};


