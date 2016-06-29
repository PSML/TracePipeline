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

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <queue>
#include <functional>
#include <algorithm>

#include "wtreebwt.h"

// -------- DynFMI --------

DynFMI::~DynFMI()
{
  deleteDynFMINodes(root);
  delete[] leaves; //free(leaves) // ???;
}

void DynFMI::deleteDynFMINodes(WaveletNode *n)
{
  if (n->right) deleteDynFMINodes(n->right);
  if (n->left) deleteDynFMINodes(n->left);
	
  delete n;
}

DynFMI::DynFMI(alpha_t *text, trace_t n)
{
  initEmptyDynFMI(text);
  addText(text, n);
}

void DynFMI::iterateReset()
{
  iterate = 1;
  recursiveIterateResetWaveletNode(root);
}

void DynFMI::recursiveIterateResetWaveletNode(WaveletNode *w)
{
  w->bittree->iterateReset();
	
  if (w->left) recursiveIterateResetWaveletNode(w->left);
  if (w->right) recursiveIterateResetWaveletNode(w->right);
}


bool DynFMI::iterateNext()
{
  iterate++;
  return !(iterate > getSize());
}

alpha_t DynFMI::iterateGetSymbol()
{
  //trace_t i = iterate;
  WaveletNode *walk = root;	
  bool bit;
		
  while (true) {
    bit = walk->bittree->iterateGetBit(); // TODO improve
    //i = walk->bittree->iterateGetRank(bit);
		
    walk->bittree->iterateNext();
    	
    if (bit) { // bit = 1
      if (walk->right == 0) return walk->c1;
      walk = walk->right;
    } else { // bit = 0
      if (walk->left == 0) return walk->c0;
      walk = walk->left;
    }		
  }	
}

alpha_t* DynFMI::getBWT()
{
  trace_t n = root->bittree->getPositions();
	
  alpha_t *text = new alpha_t[n];
	
  bool data = true;
  // old slow version:
  //for (trace_t i=1; i<=root->bittree->getPositions(); i++)
  //	text[i-1]=(*this)[i];
  
  trace_t i = 0;
	
  iterateReset();
	
  while (data) {
    text[i] = iterateGetSymbol();
    data = iterateNext();
    i++;
  }
  
  return text;
}

void DynFMI::deleteLeaves(WaveletNode *node)
{
  bool leaf = true;

  if (node->left) {
    // internal node
    leaf = false;
    deleteLeaves(node->left);
  }
  if (node->right){
    leaf = false;
    deleteLeaves(node->right);
  } 
	
  if (leaf) {
    // is a leaf, delete it!
    if (node->parent) {
      if (node == node->parent->left)
	node->parent->left = 0;
      else node->parent->right = 0;
    }
    delete node;
  }
}

void DynFMI::makeCodes(trace_t code, int bits, WaveletNode *node)
{
#ifndef NDEBUG
  if (node == node->left) {
    cout << "makeCodes: autsch" << endl;
    exit(0);
  }
#endif

  if (node->left) {
    makeCodes(code | (0 << bits), bits + 1, node->left);
    makeCodes(code | (1 << bits), bits + 1, node->right);
  } else {
    codes[node->c0] = code;
    codelengths[node->c0] = bits + 1;
  }
}

void DynFMI::appendBVTrees(WaveletNode *node)
{
  node->bittree = new BVTree();

  if (node->left) appendBVTrees(node->left);
  if (node->right) appendBVTrees(node->right);
}

void DynFMI::initEmptyDynFMI(alpha_t *text)
{
  // pointers to the leaves for select
  leaves = (WaveletNode**) new WaveletNode*[ALPHABETSIZE];
  for (salpha_t j=0; j<ALPHABETSIZE; j++) leaves[j] = 0;

  trace_t i=0;
  while (text[i]) {
    if (leaves[text[i]] == 0) {
      leaves[text[i]] = new WaveletNode(text[i]); 
    }
    leaves[text[i]]->weight++; 
    i++;
  }
  // separation symbol:
  leaves[0] = new WaveletNode((alpha_t)0); 
  leaves[0]->weight = 1;
	
  // Veli's approach:
  priority_queue<WaveletNode*, vector<WaveletNode*>, greater<WaveletNode*>> q;

  for (salpha_t j=0; j<ALPHABETSIZE; j++) {
    if (leaves[j]) {
      q.push(leaves[j]);
    }
    codes[j] = 0;
    codelengths[j] = 0;
  }
  // creates huffman shape:
  while (q.size() > 1) {
    WaveletNode *left = q.top();
    q.pop();
    WaveletNode *right = q.top();
    q.pop();
		
    q.push(new WaveletNode(left, right));
  }
  root = q.top();
  q.pop();

  makeCodes(0, 0, root);	// writes codes and codelengths

  // merge leaves	(one leaf represent two characters!)
  for (salpha_t j=0; j<ALPHABETSIZE; j++) {
    if (leaves[j]) {
      if (leaves[j]->parent->left==leaves[j]) {
	leaves[j]->parent->c0 = j;
      } else {
	leaves[j]->parent->c1 = j;
      }
      leaves[j] = leaves[j]->parent; // merge
    }
  }
  deleteLeaves(root);
	
  appendBVTrees(root);

  // array C needed for backwards search
  for (salpha_t j=0; j<ALPHABETSIZE+ALPHABETSIZE; j++) C[j] = 0;
}

void DynFMI::insert(alpha_t c, trace_t i)
{
#ifndef NDEBUG
  if (leaves[c] == 0) {
    cerr << "error: Symbol \"" << c << "\" (" << (int)c << ") is not in the code table!" << endl;;
    exit(EXIT_FAILURE);
  }
#endif
	
  trace_t level=0;
  trace_t code = codes[c];

  bool bit;
  WaveletNode *walk = root;	
		
  while (walk) {	
    bit = ((code & (1u << level)) != 0); 
		
    walk->bittree->insertBit(bit, i); // TODO improve
    i = walk->bittree->rank(bit, i);

    if (bit) { // bit = 1
      walk = walk->right;
    } else { // bit = 0
      walk = walk->left;
    }
		
    level++;		
  } // end of while
  
  trace_t j = ALPHABETSIZE + c;
  while (j > 1) {
    C[j]++;
    j = binaryTree_parent(j);
  }
  C[j]++;	
}

alpha_t DynFMI::operator[](trace_t i)
{
  WaveletNode *walk = root;	
  bool bit;
		
  while (true) {	
    bit = (*walk->bittree)[i]; //TODO improve by reducing
    i = walk->bittree->rank(bit, i);

    if (bit) { // bit = 1
      if (walk->right == 0) return walk->c1;
      walk = walk->right;
    } else { // bit = 0
      if (walk->left == 0) return walk->c0;
      walk = walk->left;
    }	
  }
  cout << endl;
  return 0;
}

trace_t DynFMI::rank(alpha_t c, trace_t i)
{
  if (i == 0) return 0;

  trace_t level = 0;
  trace_t code = codes[c];
  
  bool bit;
  WaveletNode *walk = root;	
		
  while (true) {
    bit = ((code & (1u << level)) != 0);
    //cout << "bit:" << bit << " = ((" << code << " & (1u << " << level << ")) != 0)" << endl;
		
    i = walk->bittree->rank(bit, i);
    if (bit) { // bit = 1
      if (walk->right == 0) return i;
      walk = walk->right;
    } else { // bit = 0
      if (walk->left == 0) return i;
      walk = walk->left;
    }
    level++;		
  }
  
  cerr << "error: DynFMI::rank: left while loop" << endl;
  exit(EXIT_FAILURE);
  return 0; // never
}

trace_t DynFMI::select(alpha_t c, trace_t i)
{	
  WaveletNode *walk = leaves[c];	
	
  bool bit = (walk->c1 == c);
	
  while (walk->parent) {
    i = walk->bittree->select(bit, i);
		
    bit = (walk == walk->parent->right);
    walk = walk->parent;
  }
	
  i = walk->bittree->select(bit, i);

  return i;
}

// size must include endmarker!
void DynFMI::addText(alpha_t *str, trace_t n)
{
  trace_t i = 1;
  
  insert(str[n-2], i); // insert second last character, corresponds to suffix of length 1
  //cout << "   str[" << n-2 << "]: 0x" << hex << (trace_t)str[n-2] << "\n";
  //cout << "   insert(0x" << hex << (trace_t)str[n-2] << ", " << i << ")\n";

  for (trace_t t=n-2; t>0; t--) {
    //cout << "    rank(0x" << hex << (trace_t)str[n-2] << ", " << i << "): " << rank(str[t], i) << "\n";
    i = 1 + getNumberOfSymbolsSmallerThan(str[t]) + rank(str[t], i);
    //cout << "     + getNumberOfSymbolsSmallerThan(0x" << hex << (trace_t)str[t] << "): " << getNumberOfSymbolsSmallerThan(str[t]) << "\n     + 1 = " << i << "\n";
    insert(str[t-1], i);
    //cout << "   str[" << t-1 << "]: 0x" << hex << (trace_t)str[t-1] << "\n";
    //cout << "   insert(0x" << hex << (trace_t)str[t-1] << ", " << i << ")\n";
  }
  //cout << "    rank(0x" << hex << (trace_t)str[0] << ", " << i << "): " << rank(str[0], i) << "\n";
  i = 1 + getNumberOfSymbolsSmallerThan(str[0]) + rank(str[0], i);
  //cout << "     + getNumberOfSymbolsSmallerThan(0x" << hex << (trace_t)str[0] << "): " << getNumberOfSymbolsSmallerThan(str[0]) << "\n     + 1 = " << i << "\n";
  insert(str[n-1], i);
  //cout << "   str[" << n-1 << "]: 0x" << hex << (trace_t)str[n-1] << "\n";
  //cout << "   insert(0x" << hex << (trace_t)str[n-1] << ", " << i << "): 0x" << hex << (trace_t)str[n-1] << "\n";
}

trace_t DynFMI::getNumberOfSymbolsSmallerThan(alpha_t c)
{
  salpha_t j = ALPHABETSIZE + c;
  trace_t r=0;

  while(j > 1) {
    if (binaryTree_isRightChild(j)) 
      r += C[binaryTree_left(binaryTree_parent(j))];
		
    j = binaryTree_parent(j);
  }
  return r;
}

void DynFMI::printDynFMIContent(ostream& stream)
{
  alpha_t c;
  for (trace_t i=1; i<=getSize(); i++) {
    c = (*this)[i];
    if (c == 0) c = '#';
    stream << c;
  }
}




