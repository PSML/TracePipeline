/***************************************************************************
 *   Sadakane's Compressed suffix tree                                     *
 *                                                                         *
 *   Copyright (C) 2006 by Niko Välimäki, Kashyap Dixit                    *
 *                                                                         *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef _SSTREE_H_
#define _SSTREE_H_

#include "SuffixTree.h"
#include "CSA.h"
#include "CHgtArray.h"
#include "BitRank.h"
#include "ReplacePattern.h"
#include "CRMQ.h"
#include "Tools.h"
#include "Parentheses.h"
#include "LcpToParentheses.h"
#include "MyNode.h"
#include <iostream>
#include <fstream>

// (Un)comment next line to get more/less debug information about the tree construction
//#define SSTREE_TIMER

// Requires HeapProfiler class:
//#define SSTREE_HEAPPROFILE


/**
 * Compressed suffix tree class
 *
 * Suffix tree is represented using several compressed       
 * data structures. Most important to understant the usage       
 * of the Balanced Parantheses (BP) representation of the tree   
 * hierarchy: the tree is traversed prefix order printing "("    
 * when a node is visited first time and printing ")" when       
 * a node is visited last time. E.g. "((()())())" is a tree with 
 * root having two children, its left child having two leaves,   
 * and its right child being a leaf.                             
 *                                                               
 * A node in the tree is represented by the index of the         
 * corresponding "(" in the balanced parentheses representation.
 *
 * References:
 *
 * Niko Välimäki, Wolfgang Gerlach, Kashyap Dixit, and Veli Mäkinen. 
 * Engineering a Compressed Suffix Tree Implementation, Published at 
 * 6th Workshop on Experimental Algorithms (WEA 2007), June 6-8, Italy.
 *
 * K. Sadakane. Compressed suffix trees with full functionality. Theory of 
 * Computing Systems, 2006. To appear, preliminary version available at 
 * http://tcslab.csce.kyushu-u.ac.jp/~sada/papers/cst.ps
 */

/*
// Forward declaration of class boost::serialization::access
namespace boost {
namespace serialization {
class access;
}
}
*/

class SSTree : public SuffixTree
{
 private:
  ulong n;
  CSA *sa;    
  CHgtArray *hgt;
  CRMQ *rmq;
  ulong *P;
  ReplacePattern *rpLeaf, *rpSibling;
  BitRank *br, *brLeaf, *brSibling;
  Parentheses *Pr;
  MyNode *myTree;
  const char *hostStr;

  /*
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    // Simply serialize the data members of SSTree
    ar & n;
    
    ar & myTree;
  }
  */

 public:
  /**
   * IO action for constructor SSTree(), filename given as the last parameter.
   * Defaults to no operation.
   */
  enum io_action
  {
    nop,       // No operation
    load_from, // Load from file
    save_to    // Save to file
  };

  SSTree(alpha_t *, ulong, bool = false, unsigned = 0, io_action = nop, const char * = 0, 
	 const char *hstr="localhost:8000");
  ~SSTree();
  ulong root();
  bool isleaf(ulong) ;
  ulong child(ulong, alpha_t);
  ulong firstChild(ulong);
  ulong sibling(ulong) ;
  ulong parent(ulong ) ;
  alpha_t edge(ulong, ulong) ;
  alpha_t* edge(ulong) ;
  alpha_t* pathlabel(ulong);
  alpha_t* substring(ulong, ulong);
  ulong depth(ulong);
  ulong nodeDepth(ulong);
  ulong lca(ulong, ulong);
  ulong lceLinear(alpha_t *, ulong, ulong);
  ulong lce(ulong, ulong);
  ulong sl(ulong);
  ulong inorder(ulong);
  ulong rightmost(ulong);
  ulong leftmost(ulong);
  ulong leftrank(ulong);
  ulong numberofnodes(ulong);
  ulong numberofleaves(ulong);
  ulong textpos(ulong);
  ulong isOpen(ulong);
  ulong search(alpha_t *, ulong);
  void PrintHgt();
  ulong lcaParen(ulong, ulong);
  void PrintSA();
  void PrintEdge(ulong);
  trace_t EdgeLen(trace_t);
  void CheckLCA(ulong);
  void PrintTree(ulong, int, int recursive=1);
  void InitMyTree(trace_t, trace_t);
  trace_t InitCount(trace_t);
  void InitOffsets();
  void InitOffsets(trace_t, trace_t);
  void PrintOffsets(std::list<trace_t>, trace_t, int);
  void PrintPathOffsets(std::list<trace_t>, trace_t, int, int annFmt=0);
  void PrintNodeInfo(trace_t);
  void PrintTreeInfo();
  trace_t PrintTreeInfo(MyNode);
  void PrintMyTree(trace_t, trace_t);
  void PrintDotNode(trace_t);
  void PrintDotEdge(trace_t, trace_t);
  void PrintDotPath(trace_t);
  void PrintDotSl(trace_t, trace_t);
  void PrintNodeEdgeCount(trace_t, trace_t);
  void PrintAccuracy(trace_t, int);
  void GetAccuracy(trace_t, trace_t, trace_t *);
  void GetAccuracy2(trace_t, trace_t, trace_t *, trace_t *);
  trace_t GetPathLen(trace_t);
  void PrintHotPath(trace_t);
  trace_t GetHotPath(trace_t);
  trace_t GetChildNodeOfSymbol(trace_t, alpha_t);
  trace_t GetChildNodeOfSymbols(trace_t, list<alpha_t>&);
  trace_t GetNodeOfSymbols(list<alpha_t>);
  void PrintLCPEndingIn(std::list<alpha_t>, int);
  std::list<alpha_t> GetLCPEndingIn(std::list<alpha_t>);
  void PrintList(std::list<alpha_t>, int);
  void PrintLPsEndingIn(std::list<alpha_t>, int);
  void PrintLongestPathEndingIn(alpha_t, int);
  alpha_t* GetLongestPathEndingIn(alpha_t);
  void PrintCausalSyms(std::list<trace_t>, int);
  void PrintCausalSyms(trace_t, int);
  void PrintLabel(alpha_t *, int);
  void PrintSuffixesFrom(std::list<trace_t>);
  void PrintAnnOfNodePath(MyNode);
  void PrintAnnOfNode(MyNode);
  void PrintAnnRect(trace_t, trace_t, string);
  void PrintAnnOfOffsets(list<trace_t>, string);
  void PrintAnnVline(trace_t, string);
  list<trace_t> getOffsetsFromCluster(list<MyNode>, list<trace_t>);
  void clusterChildNodesOf(trace_t, list<list<MyNode>>&, list<list<trace_t>>&);
  void getUniqOffsetsInCluster(list<MyNode>&, list<trace_t>&);
  alpha_t *addSymToSeq(alpha_t *, trace_t, trace_t);
  alpha_t *zeroOffsetsInSeq(alpha_t *, list<trace_t>);
};
  
#endif
