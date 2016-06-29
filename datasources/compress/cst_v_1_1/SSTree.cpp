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

// References:
// 
// K. Sadakane. Compressed suffix trees with full functionality. Theory of 
// Computing Systems, 2007. To appear, preliminary version available at 
// http://tcslab.csce.kyushu-u.ac.jp/~sada/papers/cst.ps

#include "SSTree.h"
#include "MyNode.h"

#ifdef SSTREE_HEAPPROFILE
#   include "HeapProfiler.h"
#endif

#include <list>
#include <sstream>
using namespace std;

/**
 * Constructor.
 *
 * Compressed suffix tree is a self-index so text is not needed after construction.
 * Text can be deleted during construction to save some construction space.
 *
 * Parameter <filename> is a prefix for filenames used for IO operation.
 * Filename suffixes are ".bwt", ".lcp" and ".bp".
 *
 * @param text a pointer to the text.
 * @param n number of symbols in the text.
 * @param deletetext delete text as soon as possible.
 * @param samplerate sample rate for Compressed suffix array.
 * @param action IO action for <filename>. Defaults to no operation.
 * @param filename Prefix for the filenames used.
 */
SSTree::SSTree(alpha_t *text, ulong n, bool deletetext, unsigned samplerate, io_action IOaction, const char *filename, const char *hstr)
{
#ifdef SSTREE_HEAPPROFILE
  ulong heapCon;
#endif
  
  this->n = n;
  unsigned floorLog2n = Tools::FloorLog2(n);
  if (floorLog2n < 4)
    floorLog2n = 4;
  unsigned rmqSampleRate = floorLog2n / 2;
  if (samplerate != 0)
    floorLog2n = samplerate; // Samplerate override, affects only CSA
    
#ifdef SSTREE_TIMER
#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating CSA with samplerate %u\n", floorLog2n);
  fflush(stdout);
  Tools::StartTimer();    
#endif
  if (IOaction == load_from && filename != 0)
    sa = new CSA(text, n, floorLog2n, (string(filename)+".csa").c_str()); 
  else if (IOaction == save_to && filename != 0)
    sa = new CSA(text, n, floorLog2n, 0, (string(filename)+".csa").c_str()); 
  else // No IO operation
    sa = new CSA(text, n, floorLog2n); 
#ifdef SSTREE_TIMER
  printf("CSA created in %.0f seconds.\n", Tools::GetTime());

#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl; 
  heapCon = HeapProfiler::GetHeapConsumption();   
#endif

  printf("Creating CHgtArray\n");
  fflush(stdout);
  Tools::StartTimer();    
#endif
  if (IOaction == load_from && filename != 0)
    hgt = new CHgtArray(sa, (string(filename)+".lcp").c_str());
  else
    hgt = new CHgtArray(sa, text, n);

  if (IOaction == save_to && filename != 0)
    hgt->SaveToFile((string(filename)+".lcp").c_str());
#ifdef SSTREE_TIMER
  printf("CHgtArray created in %.0f seconds.\n", Tools::GetTime());
   
#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating parentheses sequence (LcpToParentheses)\n");
  fflush(stdout);
  Tools::StartTimer();
#endif
  if (deletetext)
    delete [] text;
    
#ifdef SSTREE_HEAPPROFILE
  heapCon = HeapProfiler::GetHeapConsumption(); 
#endif
    
  ulong bitsInP;
  if (IOaction == load_from && filename != 0)
    P = LcpToParentheses::GetBalancedParentheses((string(filename)+".bp").c_str(), bitsInP);
  else
    P = LcpToParentheses::GetBalancedParentheses(hgt, n, bitsInP);

  if (IOaction == save_to && filename != 0)
    LcpToParentheses::SaveToFile((string(filename)+".bp").c_str(), P, bitsInP);
#ifdef SSTREE_TIMER
  printf("Parentheses sequence created in %.0f seconds.\n", Tools::GetTime());
    
#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  //printf("Creating CSA with sample rate %d\n", floorLog2n);
  //Tools::StartTimer();    
#endif
  //delete sa;
  //sa = new CSA(text, n, floorLog2n);
  //hgt->SetSA(sa);  // Update SA pointer
#ifdef SSTREE_TIMER
  //printf("CSA created in %f seconds.\n", Tools::GetTime());

  printf("Creating BitRank\n");
  fflush(stdout);
  Tools::StartTimer();    
#endif
  br = new BitRank(P, bitsInP, false);
#ifdef SSTREE_TIMER
  printf("BitRank created in %.0f seconds.\n", Tools::GetTime());

#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating Parentheses\n");
  fflush(stdout);
  Tools::StartTimer();    
#endif
  Pr = new Parentheses(P, bitsInP, true, br);
#ifdef SSTREE_TIMER
  printf("Parentheses created in %.0f seconds.\n", Tools::GetTime());

#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating ReplacePatterns\n");
  fflush(stdout);
  Tools::StartTimer();    
#endif
  rpLeaf = new ReplacePattern(1, 8);
  rpSibling = new ReplacePattern(0, 8);
#ifdef SSTREE_TIMER
  printf("ReplacePatterns created in %.0f seconds.\n", Tools::GetTime());

#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating BitRanks\n");
  fflush(stdout);
  Tools::StartTimer();    
#endif
  brLeaf = new BitRank(P, bitsInP, false, rpLeaf);       //for ()
  brSibling = new BitRank(P, bitsInP, false, rpSibling); //for )(
    
  if (rmqSampleRate < 4)
    rmqSampleRate = 4;
#ifdef SSTREE_TIMER
  printf("BitRanks created in %.0f seconds.\n", Tools::GetTime());
  //         printf("<enter>\n");
  //         cin.get();

#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
  heapCon = HeapProfiler::GetHeapConsumption();
#endif

  printf("Creating CRMQ with sample rates %d, %d and %d\n", rmqSampleRate * rmqSampleRate * rmqSampleRate, rmqSampleRate * rmqSampleRate, rmqSampleRate);
  Tools::StartTimer();    
  fflush(stdout);
#endif
  rmq = new CRMQ(br, P, bitsInP, rmqSampleRate * rmqSampleRate * rmqSampleRate, rmqSampleRate * rmqSampleRate, rmqSampleRate);
#ifdef SSTREE_TIMER
#ifdef SSTREE_HEAPPROFILE
  cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << endl;
#endif
  printf("CRMQ created in %.0f seconds.\n", Tools::GetTime());
  fflush(stdout);
#endif

  trace_t maxID = rightmost(root());
  myTree = new MyNode[maxID+1];
  InitMyTree(root(), 0);
  InitCount(root());
  InitOffsets();
  hostStr = hstr;
}

/**
 * A destructor. 
 */
SSTree::~SSTree()
{
  delete rmq;
  delete sa;
  delete rpLeaf;
  delete rpSibling;
  delete brLeaf;
  delete brSibling;
  delete br;
  delete hgt;
  delete Pr;
  delete [] P;
  delete [] myTree;
}

/**
 * Returns the position of the root node in the parentheses sequence.
 */
ulong SSTree::root()
{
  return 0;
}

/** 
 * Check if node v is a leaf node.
 *
 * Method doesn't check for an open parenthesis at position v (because we can assume that v is a node).
 */
bool SSTree::isleaf(ulong v)
{
  return !br->IsBitSet(v + 1);
}

/**
 * Returns the child node of v so that edge leading to the child node starts with the symbol c.
 */
ulong SSTree::child(ulong v, alpha_t c)
{
  if (isleaf(v))
    return 0;
        
  v++;   // First child of v
  while (v != 0)
    {
      if (c == edge(v,1)) 
	return v;
      v = sibling(v);
    }
  return 0;
}

/**
 * Returns the first child of the (internal) node v.
 */
ulong SSTree::firstChild(ulong v)
{
  if (isleaf(v))
    return 0;
        
  return v+1;
}

/**
 * Returns the next sibling of the node v.
 */
ulong SSTree::sibling(ulong v)
{
  if (v == 0)
    return 0;

  ulong w = Pr->findclose(parent(v));
  ulong i = Pr->findclose(v) + 1;
  if (i < w)
    return i;

  return 0;   // Returns zero if no next sibling
}

/**
 * Returns the parent of the node v.
 */
ulong SSTree::parent(ulong v)
{
  return Pr->enclose(v);
}

/**
 * Returns the d-th character of the label of the node v's incoming edge.
 */
alpha_t SSTree::edge(ulong v, trace_t d)
{
  alpha_t *ss; 
  if (isleaf(v)) {
    ulong i = leftrank(v);
    ulong j = depth(parent(v));
    ulong d1 = sa->lookup(i) + j;
    if (d > n - d1)
      return 0u;
    ss = sa->substring(d1 + d - 1, 1);
    alpha_t result = ss[0];
    delete [] ss;
    return result;
  }
    
  ulong d1 = hgt->GetPos(inorder(parent(v)));
  ulong d2 = hgt->GetPos(inorder(v));
  if (d > d2 - d1)
    return 0u;
  ss = sa->substring(sa->lookup(inorder(v)) + d1 + d - 1,1);
  alpha_t result = ss[0];
  delete [] ss;
  return result;    
}

/**
 * Returns the edge label of the incoming edge of the node v.
 */
alpha_t* SSTree::edge(ulong v) 
{
  if (isleaf(v)) {
    ulong i = leftrank(v);
    ulong j = depth(parent(v));
    ulong k = depth(v);
    ulong d1 = sa->lookup(i) + j;
    return sa->substring(d1,k-j);
  }
    
  ulong d1 = hgt->GetPos(inorder(parent(v)));
  ulong d2 = hgt->GetPos(inorder(v));
   
  return sa->substring(sa->lookup(inorder(v))+d1, d2-d1);
}

/**
 * Returns the path label from root to the node v.
 */
alpha_t* SSTree::pathlabel(ulong v) 
{
  if (isleaf(v))
    {
      ulong i = leftrank(v);
      ulong k = depth(v);
      ulong d1 = sa->lookup(i);
      return sa->substring(d1, k);
    }
  ulong d2 = hgt->GetPos(inorder(v));
  return sa->substring(sa->lookup(inorder(v)), d2);
}

/**
 * Returns a substring of the original text sequence.
 *
 * @param i starting position.
 * @param k length of the substring.
 */
alpha_t *SSTree::substring(ulong i, ulong k)
{
  return sa->substring(i,k);
}

/**
 * Returns the string depth of the node v.
 */
ulong SSTree::depth(ulong v)
{
  if (v == 0)
    return 0;

  if (isleaf(v))
    {
      ulong i = leftrank(v);
      return n - sa->lookup(i);
    }
        
  v = inorder(v);
  return hgt->GetPos(v);  
}

/**
 * Returns the node depth of the node v.
 *
 * Node depth for the root node is zero.
 */
ulong SSTree::nodeDepth(ulong v)
{
  return 2 * br->rank(v) - v - 2;
}

/**
 * Returns the Lowest common ancestor of nodes v and w.
 */
ulong SSTree::lca(ulong v, ulong w)
{
  if (v == 0 || w == 0)
    return 0;

  if (v == w || v == root())
    return v;
  if (w == root())
    return root();

  if (v > w)
    {
      ulong temp = w;
      w = v;
      v = temp;
    }
    
  if (Pr->findclose(v) > Pr->findclose(w))
    return v;
    
  return parent(rmq->lookup(v, w) + 1);
}


/**
 * Longest common extension of text positions i and j.
 *
 * Linear time solution.
 */
ulong SSTree::lceLinear(alpha_t *text, ulong i, ulong j)
{
  ulong k = 0;
  while (text[i+k] == text[j+k])
    k++;
    
  return k;
}

/**
 * Returns the Longest common extension of text positions i and j.
 */
ulong SSTree::lce(ulong i, ulong j)
{
  i = sa->inverse(i);
  ulong v = brLeaf->select(i + 1);
  j = sa->inverse(j);
  ulong w = brLeaf->select(j + 1);
  return depth(lca(v, w));
}

/**
 * Suffix link for internal nodes
 */
ulong SSTree::sl(ulong v)
{
  if (v == 0 || v == root() || isleaf(v))
    return 0;
    
  ulong x = brLeaf->rank(v - 1) + 1;
  ulong y = brLeaf->rank(Pr->findclose(v));
  x = sa->Psi(x - 1);
  y = sa->Psi(y - 1);
  return lca(brLeaf->select(x + 1), brLeaf->select(y + 1));
}

/**
 * Prints Hgt array values
 */
void SSTree::PrintHgt()
{
  cout << "Hgt: [ ";
  for (ulong i = 0; i < n; i++)
    cout << i << "=" << hgt->GetPos(i) << " ";
  cout << "]\n";
}

/**
 * Prints Suffix array values
 */
void SSTree::PrintSA()
{
  cout << "SA: [ ";
  for (trace_t i = 0; i < n; i++)
    cout << i << "=" << sa->lookup(i) << " ";
  cout << "]\n";
}

/**
 * Prints the edge label of the incoming edge
 */
void SSTree::PrintEdge(trace_t v)
{
  trace_t k = 1;
  alpha_t c = edge(v, k);

  cout << v << ":";
  while (c) {
    cout << " " << c;
    k++;
    c = edge(v, k);
  }
  cout << endl;
}

trace_t SSTree::EdgeLen(trace_t v)
{
  if (v == 0) return 0;
  
  if (isleaf(v)) {
    ulong j = depth(parent(v));
    ulong k = depth(v);
    return k - j - 1;
  }
  ulong d1 = hgt->GetPos(inorder(parent(v)));
  ulong d2 = hgt->GetPos(inorder(v));
  return d2 - d1;
}

/**
 * Returns the Lowest common ancestor of nodes v and w.
 *
 * Linear time solution for debuging.
 */
ulong SSTree:: lcaParen(ulong v, ulong w)
{
      
  ulong temp;
  if (v < w) temp = Pr->findclose(w);
  else temp = Pr->findclose(v);
  if (v == w) return w;
  if (v > w)
    v = w;
    
  while (v > 0)
    {
      if (Pr->findclose(v) > temp) return v; 
      v = parent(v); 
    }
    
  return 0;
}

/**
 * Debug function for Lowest common ancestor
 */
void SSTree::CheckLCA(ulong v)
{
  ulong len = br->rank(rightmost(v));
  v++;ulong w, temp, v1;
  for (w = 1; w < len - 1; w++)
    {
      temp = br->select(w);
      v1 = br->select(w+1);
      while(v1 < len)
        {   
	  if (lca(temp, v1) != lcaParen(temp, v1))
            {
	      // printf("conflict at (%lu, %lu)::lcaParen() = %lu and lca() = %lu\n", temp, v1, lcaParen(temp, v1), lca(temp, v1));
	      cerr << "conflict at (" << temp << ", " << v1 << ")::lcaParen() = " << lcaParen(temp, v1) << " and lca() = " << lca(temp, v1) << endl;
	      exit(0);
            }
	  v1 = br->select(br->rank(v1)+1);
        }
        
      // Check for the value v1 == len
      if (lca(temp, v1) != lcaParen(temp, v1))
        {
	  // printf("conflict at (%lu, %lu)::lcaParen() = %lu and lca() = %lu\n", temp, v1, lcaParen(temp, v1), lca(temp, v1));
	  cerr << "conflict at (" << temp << ", " << v1 << ")::lcaParen() = " << lcaParen(temp, v1) << " and lca() = " << lca(temp, v1) << endl;
	  exit(0);
        }        
    }
}

/**
 * Prints edge labels of the Suffix tree
 */
void SSTree::PrintTree(ulong v, int depth, int recursive)
{
  for (int i=0; i<depth; i++)
    printf(" ");
   
  if (v) PrintEdge(v);
  
  if (!(isleaf(v)) && recursive)
    PrintTree(v + 1, depth + 1,1);

  if (!(isleaf(v)) && recursive==0 && depth==0)
    PrintTree(v + 1, depth + 1, 0);

  v = sibling(v);
  if (v) PrintTree(v, depth, recursive);
}

void SSTree::InitMyTree(trace_t v, trace_t p)
{
  trace_t e = EdgeLen(v);
  
  if (v == root() || e) {
    myTree[v].id = v;
    myTree[v].parent = p;
    if (v) myTree[p].children.push_back(v);
    
    myTree[v].edgeLen = e;
    trace_t t = sl(v);
    if (t) {
      myTree[v].slinkTo = t;
      myTree[t].slinkFrom.push_back(v);
    }    
    if (isleaf(v))
      myTree[v].count = 1;
  }
  if (!isleaf(v))
    InitMyTree(v + 1, v);
  
  v = sibling(v);
  if (v) InitMyTree(v, p);
}

trace_t SSTree::InitCount(trace_t v)
{
  if (myTree[v].count > 0)
    return myTree[v].count;

  list<trace_t>::iterator it;  
  if (myTree[v].children.empty()) {
    myTree[v].count = 1;
    return 1;
  }
  for (it=myTree[v].children.begin();
       it!=myTree[v].children.end(); it++) {
    trace_t c = *it;
    myTree[v].count += InitCount(c);
  }
  return myTree[v].count;
}

void SSTree::InitOffsets(trace_t v, trace_t len)
{
  if (myTree[v].children.empty()) {
    myTree[v].offsets.push_back(len - myTree[v].edgeLen);
    return;
  }
  trace_t c, s;
  list<trace_t>::iterator it_c, it_s;
  for (it_c=myTree[v].children.begin();
       it_c!=myTree[v].children.end(); it_c++) {
    c = *it_c;
    InitOffsets(c, len);
  }
  for (it_c=myTree[v].children.begin();
       it_c!=myTree[v].children.end(); it_c++) {
    c = *it_c;
    for (it_s=myTree[c].offsets.begin();
	 it_s!=myTree[c].offsets.end(); it_s++) {
      s = *it_s;
      myTree[v].offsets.push_back(s - myTree[v].edgeLen);
    }
  }
}

void SSTree::InitOffsets()
{
  list<trace_t>::iterator it;
  for (it=myTree[root()].children.begin();
       it!=myTree[root()].children.end(); it++)
    InitOffsets(*it, n-1); // end symbol 0u
}

void SSTree::PrintOffsets(list<trace_t> nlist, trace_t depth, int prune)
{
  if (nlist.empty()) return;

  list<trace_t>::iterator it_v, it_o;
  for (it_v=nlist.begin(); it_v!=nlist.end(); it_v++) {
    MyNode node = myTree[*it_v];
    if (*it_v != root()) {
      if (prune) {
	if (node.edgeLen == 1) continue;
	if (node.offsets.size() < 2) continue;
      }
      cout << *it_v << " ";
      cout << node.edgeLen << " :";
      for (it_o=node.offsets.begin();
	   it_o!=node.offsets.end(); it_o++) {
	cout << " " << *it_o;
      }
      cout << endl;
    }
    if (depth)
      PrintOffsets(node.children, depth-1, prune);
  }
}

void SSTree::PrintPathOffsets(list<trace_t> nlist, trace_t depth, int prune, int annFmt)
{
  if (nlist.empty()) return;

  list<trace_t>::iterator it_v, it_o;
  for (it_v=nlist.begin(); it_v!=nlist.end(); it_v++) {
    trace_t v = *it_v;
    MyNode node = myTree[v];
    if (v != root()) {
      if (prune) {
	if (node.edgeLen == 1) continue;
	if (node.offsets.size() < 2) continue;
      }
      //cout << v << " ";

      trace_t plen=0;
      MyNode cur = node;
      while (cur.parent) {
	cur = myTree[cur.parent];
	plen += cur.edgeLen;
      }
      if (annFmt) {
	it_o = node.offsets.begin();
	if (it_o!=node.offsets.end()) {
	  cout << "s " << *it_o - plen << endl;
	}
	for (;
	     it_o!=node.offsets.end(); it_o++) {
	  trace_t offset = *it_o - plen;
	  cout << "r " << offset << " 0 " << node.edgeLen + plen << " max 0 1 0 0.15 " << *it_v << endl;
	}
      } else {
	cout << *it_v << " ";
	cout << node.edgeLen + plen << " :";
	for (it_o=node.offsets.begin();
	     it_o!=node.offsets.end(); it_o++) {
	  trace_t offset = *it_o - plen;
	  cout << " " << offset;
	}
	cout << endl;
      }
    }
    if (depth)
      PrintPathOffsets(node.children, depth-1, prune, annFmt);
  }
}

void SSTree::PrintNodeInfo(trace_t v)
{
  MyNode n = myTree[v];
  cout << "Node " << v << ":" << endl;
  if (v == root())
    cout << "  root node" << endl;
  else {
    cout << "  parent: " << n.parent << endl;
    cout << "  edge length: " << n.edgeLen << endl;
  }
  cout << "  count: " << n.count << endl;
  if (n.slinkTo)
    cout << "  suffix link: " << v << " -> " << n.slinkTo << endl;
  list<trace_t>::iterator it;
  for (it=n.slinkFrom.begin(); it!=n.slinkFrom.end(); it++)
    cout << "  suffix link: " << *it << " -> " << v << endl;
  cout << "  number of children: " << n.children.size() << endl;
}

trace_t SSTree::PrintTreeInfo(MyNode n)
{
  trace_t nCnt = 1;
  for (list<trace_t>::iterator it=n.children.begin();
       it!=n.children.end(); it++) {
    trace_t c = *it;
    nCnt += PrintTreeInfo(myTree[c]);
  }
  return nCnt;
}

void SSTree::PrintTreeInfo()
{
  MyNode n = myTree[root()];
  trace_t nCnt = 1;
  trace_t maxDepth = 1;

  for (list<trace_t>::iterator it=n.children.begin();
       it!=n.children.end(); it++) {
    trace_t c = *it;
    nCnt += PrintTreeInfo(myTree[c]);
  }
  cout << "Number of nodes: " << nCnt << endl;
  cout << "Maximum depth: " << maxDepth << endl;
  cout << "Number of nodes at each depth: " << endl;
}

void SSTree::PrintDotEdge(trace_t p, trace_t c)
{
  cout << p << " -> " << c
	    << " [label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"4\">"
	    << "<TR><TD target=\"_graphviz\" href=\"http:" << hostStr << "/txt?-n" << c << "%20-E\">" << myTree[c].edgeLen << "</TD></TR></TABLE>>];"
	    << endl;
}

void SSTree::PrintDotSl(trace_t f, trace_t t)
{
  cout << f << " -> " << t << " [style=dotted constraint=false];" << endl;
}

void SSTree::PrintDotNode(trace_t v)
{
  MyNode n = myTree[v];
  trace_t c = n.children.size();

  cout << v
	    << " [label=<<TABLE";
  if (c==0) cout << " BGCOLOR=\"grey\"";
  if (v==0) cout << " BORDER=\"4\"";
  else cout      << " BORDER=\"0\"";
  cout << " CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">"
	    << "<TR>";
  if (v==0) cout << "<TD COLSPAN=\"1\" target=\"_blank\" href=\"http:" << hostStr << "/sftclr?-n" << v << "%20-N\">" << v << "</TD>";
  else cout      << "<TD COLSPAN=\"3\" target=\"_blank\" href=\"http:" << hostStr << "/sftclr?-n" << v << "%20-N\">" << v << "</TD>";
  cout << "<TD COLSPAN=\"2\" target=\"_blank\" href=\"http:" << hostStr << "/sft?-n" << v << "%20-p%20-N\">" << myTree[v].count << "</TD>"
	    << "</TR>"
	    << "<TR>";
  if (v==0) cout << "<TD></TD>";
  else cout      << "<TD target=\"_blank\" href=\"http:" << hostStr << "/sft?-n" << v << "%20-S\">s</TD>"
		      << "<TD target=\"_blank\" href=\"http:" << hostStr << "/sft?-n" << v << "%20-H\">h</TD>"
		      << "<TD target=\"_blank\" href=\"http:" << hostStr << "/txt?-n" << v << "%20-A\">c</TD>";
  if (c) cout << "<TD target=\"_blank\" href=\"http:" << hostStr << "/sft?-n" << v << "%20-d1%20-T\">" << c << "</TD>";
  else cout   << "<TD>0</TD>";
  cout << "<TD target=\"_blank\" href=\"http:"<< hostStr << "/plt?-n" << v << "%20-F\">p</TD>"
	    << "</TR>"
	    << "</TABLE>"
	    << ">];" << endl;
}

void SSTree::PrintMyTree(trace_t v, trace_t depth)
{
  list<trace_t>::iterator it;

  PrintDotNode(v);

  for (it=myTree[v].children.begin();
       it!=myTree[v].children.end(); it++) {
    trace_t c = *it;
    PrintDotEdge(v, c);

    /* FIXME: only print suffix links if printing the whole tree */
    if (!depth) {
      trace_t t = myTree[c].slinkTo;
      if (t) PrintDotSl(c, t);
    }
    
    if (!depth)
      PrintMyTree(c, 0);
    else if (depth == 1)
      PrintDotNode(c);
    else if (depth > 1)
      PrintMyTree(c, depth-1);
  }
}

void SSTree::PrintNodeEdgeCount(trace_t v, trace_t depth)
{
  list<trace_t>::iterator it;
  for (it=myTree[v].children.begin();
       it!=myTree[v].children.end(); it++) {
    trace_t c = *it;
    cout << c << " " << myTree[c].edgeLen << " " << myTree[c].count << endl;
    
    //printf("%lu %lu %lu\n", c, myTree[c].edgeLen, myTree[c].count);
    
    if (!depth)
      PrintNodeEdgeCount(c, 0);
    else if (depth > 1)
      PrintNodeEdgeCount(c, depth-1);
  }
}

/**
   Accuracy(n) returns the probability that a path p of a
   randomly selected leaf will not have an internal vertex
   located after the nth symbol of concatenated labels
**/
void SSTree::GetAccuracy(trace_t v, trace_t edgeLen,
			 trace_t *numerator)
{
  list<trace_t>::iterator it;
  for (it=myTree[v].children.begin();
       it!=myTree[v].children.end(); it++) {
    trace_t c = *it;
    trace_t clen = myTree[c].edgeLen;
    if (clen > edgeLen)
      *numerator += myTree[c].count;
    else if (clen < edgeLen && !isleaf(c))
      GetAccuracy(c, edgeLen - clen, numerator);
  }
}

/**
   Accuracy(n) returns the probability that a path p
   (of length greater than n) of a randomly selected
   leaf will not have an internal vertex located after
   the nth symbol of concatenated labels
**/
void SSTree::GetAccuracy2(trace_t v, trace_t edgeLen,
			  trace_t *numerator, trace_t *denominator)
{
  list<trace_t>::iterator it;
  for (it=myTree[v].children.begin();
       it!=myTree[v].children.end(); it++) {
    trace_t c = *it;
    trace_t clen = myTree[c].edgeLen;
    if (clen > edgeLen)
      *numerator += myTree[c].count;
    else if (isleaf(c))
      *denominator -= 1;
    else if (clen < edgeLen)
      GetAccuracy2(*it, edgeLen - clen, numerator, denominator);
  }
}

void SSTree::PrintAccuracy(trace_t edgeLen, int c)
{
  trace_t numerator = 0;
  trace_t denominator = myTree[root()].count;
  
  if (c == 2)
    GetAccuracy2(root(), edgeLen, &numerator, &denominator);
  else
    GetAccuracy(root(), edgeLen, &numerator);

  cout << numerator << " " << denominator << endl;
}

trace_t SSTree::GetPathLen(trace_t v)
{
  MyNode node = myTree[v];
  trace_t curLen = node.edgeLen;
  while (node.parent != root()) {
    node = myTree[node.parent];
    curLen += node.edgeLen;
  }
  return curLen;
}

void SSTree::PrintDotPath(trace_t v)
{
  if (v == root()) return;

  trace_t c = v;
  trace_t p = myTree[v].parent;

  PrintDotNode(c);
  while (p != root()) {
    PrintDotNode(p);
    PrintDotEdge(p, c);
    c = p;
    p = myTree[c].parent;
  }
  PrintDotNode(root());
  PrintDotEdge(root(), c);
}

trace_t SSTree::GetHotPath(trace_t v)
{
  if (v == root()) return 0;

  trace_t f=0;
  list<trace_t> flist = myTree[v].slinkFrom;
  while (flist.size() == 1) {
    if (myTree[flist.front()].count != myTree[v].count)
      return f;
    f = flist.front();
    flist = myTree[f].slinkFrom;
  }
  return f;
}


void SSTree::PrintHotPath(trace_t v)
{
  if (v == root()) return;

  PrintDotPath(v);

  trace_t f = GetHotPath(v);
  if (f) {
    PrintDotSl(f, v);
    PrintDotPath(f);
  }
}

void SSTree::PrintLabel(alpha_t *label, int txt)
{
  if (label == NULL) return;

  alpha_t s = *label;
  while (s) {
    if (txt) {
      cout << " " << s;
    } else {
      cout << "  <TD><a href=\"http:" << hostStr << "/dict?" << s << "\"> " << s << "</a></TD>" << endl;
    }
    label++;
    s = *label;
  }
}

void SSTree::PrintCausalSyms(trace_t v, int txt)
{
  if (v == root()) return;

  if (txt) {
    cout << v << " :";
  } else {
    cout << "<TR><TD><a href=\"http:" << hostStr << "/plt?" << v << "\">" << v << "</a> :</TD>" << endl;
  }
  PrintLabel(pathlabel(parent(v)), txt);

  if (txt) {
    cout << " " << edge(v, 1) << endl;
  } else {
    cout << "  <TD><a href=\"http:" << hostStr << "/dict?" << v << "\"> " << edge(v, 1) << "</a></TD>" << endl;
    cout << "</TR>" << endl;
  }
}

void SSTree::PrintCausalSyms(list<trace_t> nlist, int txt)
{
  if (nlist.empty()) return;

  if (!txt) {
    cout << "<TABLE>" << endl;
  }
  list<trace_t>::iterator it;
  for (it=nlist.begin(); it!=nlist.end(); it++) {
    PrintCausalSyms(*it, txt);
  }
  if (!txt) {
    cout << "</TABLE>" << endl;
  }
}

trace_t SSTree::GetChildNodeOfSymbol(trace_t v, alpha_t s)
{
  MyNode n = myTree[v];
  for (list<trace_t>::iterator it=n.children.begin();
       it!=n.children.end(); it++) {
    trace_t c = *it;
    if (s == edge(c, 1)) return c;
  }
  return 0;
}

trace_t SSTree::GetChildNodeOfSymbols(trace_t v, list<alpha_t>& syms)
{
  trace_t c = GetChildNodeOfSymbol(v, syms.front());
  syms.pop_front();

  trace_t k = 1;
  alpha_t s = edge(c, k);
  while (s) {
    if (s != syms.front()) return 0;
    else syms.pop_front();
    k++;
    s = edge(c, k);
  }
  return c;
}

trace_t SSTree::GetNodeOfSymbols(list<alpha_t> syms)
{
  trace_t v = GetChildNodeOfSymbols(root(), syms);

  while (syms.size()) {
    v = GetChildNodeOfSymbols(v, syms);
  }
  return v;
}

alpha_t* SSTree::GetLongestPathEndingIn(alpha_t s)
{
  trace_t v = GetChildNodeOfSymbol(root(), s);
  if (v) v = GetHotPath(v);
  if (v) v = parent(v);
  if (v) return pathlabel(v);
  return NULL;
}

void SSTree::PrintLongestPathEndingIn(alpha_t s, int txt)
{
  alpha_t *label = GetLongestPathEndingIn(s);
  if (label == NULL) return;

  if (txt) {
    cout << s << " :";
  } else {
    cout << "<TR><TD><a href=\"http:" << hostStr << "/plt?" << s << "\">" << s << "</a> :</TD>" << endl;
  }
  PrintLabel(label, txt);

  if (txt) {
    cout << endl;
  } else {
    cout << "</TR>" << endl;
  }
}

void SSTree::PrintLPsEndingIn(list<alpha_t> slist, int txt)
{
  if (slist.empty()) return;

  if (!txt) {
    cout << "<TABLE>" << endl;
  }
  list<alpha_t>::iterator it;
  for (it=slist.begin(); it!=slist.end(); it++) {
    PrintLongestPathEndingIn(*it, txt);
  }
  if (!txt) {
    cout << "</TABLE>" << endl;
  }
}

void SSTree::PrintList(list<alpha_t> slist, int txt)
{
  for (list<alpha_t>::iterator it=slist.begin();
       it!=slist.end(); it++) {
    if (txt) {
      cout << " " << *it;
    } else {
      cout << "  <TD><a href=\"http:" << hostStr << "/dict?" << *it << "\"> " << *it << "</a></TD>" << endl;
    }
  }
}

list<alpha_t> SSTree::GetLCPEndingIn(list<alpha_t> slist)
{
  list<alpha_t> rlist;
  if (slist.empty()) return rlist;

  list<list<alpha_t>> paths;
  for (list<alpha_t>::iterator it=slist.begin();
       it!=slist.end(); it++) {
    alpha_t *label = GetLongestPathEndingIn(*it);
    if (label == NULL) continue;

    list<alpha_t> p;
    alpha_t s = *label;
    while (s) {
      p.push_back(s);
      label++;
      s = *label;
    }
    if (!paths.empty() && p.size() < paths.front().size()) {
      paths.push_front(p);
    } else {
      paths.push_back(p);
    }
  }
  list<alpha_t> first = paths.front(); paths.pop_front();
  trace_t sIdx = first.size();
  for (list<alpha_t>::reverse_iterator rit=first.rbegin();
       rit!=first.rend(); rit++) {
    for (list<list<alpha_t>>::iterator it=paths.begin();
    	 it!=paths.end(); it++) {
      if (*rit != (*it).back()) {
	goto printLCP;
      }
      (*it).pop_back();
    }
    sIdx--;
  }
 printLCP:
  if (sIdx == first.size()) {
    cerr << "No Longest Common Path found." << endl;
    return rlist;
  }
  trace_t cIdx = 0;
  for (list<alpha_t>::iterator it=first.begin();
       it!=first.end(); it++) {
    if (cIdx < sIdx) {
      cIdx++;
      continue;
    }
    rlist.push_back(*it);
  }
  return rlist;
}

void
SSTree::PrintLCPEndingIn(list<alpha_t> slist, int txt)
{
  if (!txt) {
    cout << "<TABLE>" << endl;
    cout << "<TR><TD>LCP :</TD>" << endl;
  } else {
    cout << "LCP :";
  }
  PrintList(GetLCPEndingIn(slist), txt);

  if (!txt) {
    cout << "</TR>" << endl;
    cout << "</TABLE>" << endl;
  } else {
    cout << endl;
  }
}

void
SSTree::PrintSuffixesFrom(list<trace_t> nlist)
{
  if (nlist.empty()) return;

  for (list<trace_t>::iterator nIt=nlist.begin();
       nIt!=nlist.end(); nIt++) {
    trace_t c = *nIt;
    if (c == root()) continue;
    list<trace_t> flist = myTree[c].slinkFrom;
    for (list<trace_t>::iterator fIt=flist.begin();
	 fIt!=flist.end(); fIt++) {
      PrintDotSl(*fIt, c);
      PrintDotNode(*fIt);
    }
  }
}

void
SSTree::getUniqOffsetsInCluster(list<MyNode>& cluster, list<trace_t>& offsets)
{
  alpha_t **cEdges = (alpha_t **)malloc(cluster.size() * sizeof(alpha_t *));

  int i=0;
  for (list<MyNode>::iterator it=cluster.begin();
       it!=cluster.end(); it++) {
    cEdges[i] = edge((*it).id);
    i++;
  }
  trace_t len = cluster.front().edgeLen;
  for (trace_t l=0; l<len; l++) {
    alpha_t s = *(cEdges[0]+l);
    for (i=1; i<cluster.size(); i++) {
      if (*(cEdges[i]+l) != s) {
	*(cEdges[0]+l) = 0;
	i = cluster.size();
      }
    }
  }
  for (trace_t l=0; l<len; l++) {
    if (*(cEdges[0]+l) == 0) {
      offsets.push_back(l);
    }
  }
  free(cEdges);
}

bool
compareEdgeLen(MyNode n1, MyNode n2)
{
  return (n1.edgeLen < n2.edgeLen);
}

void
SSTree::clusterChildNodesOf(trace_t v, list<list<MyNode>>& clusters,
			    list<list<trace_t>>& rOffsets)
{
  list<MyNode> cnodes;
  MyNode node = myTree[v];
  for (list<trace_t>::iterator it=node.children.begin();
       it!=node.children.end(); it++) {
    trace_t c = *it;
    if (!isleaf(c)) {
      //clusterChildNodesOf(c, clusters, rOffsets);
      cnodes.push_back(myTree[c]);
    }
  }
  if (cnodes.size() < 2) return;

  cnodes.sort(compareEdgeLen);

  trace_t prev = 0;
  list<MyNode>::iterator start=cnodes.begin();
  for (list<MyNode>::iterator it=cnodes.begin();
       it!=cnodes.end(); it++) {
    if (prev == 0) {
      prev = (*it).id;
      continue;
    }
    trace_t cur = (*it).id;
    if (myTree[cur].edgeLen - myTree[prev].edgeLen > 0) {  // FIXME: could relax this
      list<MyNode> cluster;
      list<trace_t> offsets;
      cluster.splice(cluster.begin(), cnodes, start, it);
      getUniqOffsetsInCluster(cluster, offsets);
      if (cluster.size() > 1) {
  	clusters.push_back(cluster);
	rOffsets.push_back(offsets);
      }
      start = it;
    }
    prev = cur;
  }
}

list<trace_t>
SSTree::getOffsetsFromCluster(list<MyNode> cluster, list<trace_t> rOffsets)
{
  // cout << "uniq offsets relative to cluster:";
  // for (list<trace_t>::iterator rIt=rOffsets.begin();
  //      rIt!=rOffsets.end(); rIt++) {
  //   cout << " " << *rIt;
  // }
  // cout << endl;

  list<trace_t> offsets;
  //cout << "cluster offsets:" << endl;
  //cout << "============================================" << endl;
  for (list<MyNode>::iterator cIt=cluster.begin();
       cIt!=cluster.end(); cIt++) {
    list<trace_t> nodeOffsets = (*cIt).offsets;
    for (list<trace_t>::iterator oIt=nodeOffsets.begin();
	 oIt!=nodeOffsets.end(); oIt++) {
      for (list<trace_t>::iterator rIt=rOffsets.begin();
	   rIt!=rOffsets.end(); rIt++) {
	//cout << " " << ((*oIt) + *(rIt));
	offsets.push_back((*oIt) + *(rIt));
      }
      //cout << endl;
    }
    //cout << endl;
  }
  //cout << "============================================" << endl;
  return offsets;
}

alpha_t *
SSTree::addSymToSeq(alpha_t *text, trace_t n, trace_t v)
{
  alpha_t maxID=0;
  for (trace_t i=0; i<n; i++) {
    alpha_t s = *(text+i);
    if ((signed)s != -1 && maxID < s) maxID = s;
  }
  maxID++;
  trace_t plen = GetPathLen(v);
  MyNode node = myTree[v];
  for (list<trace_t>::iterator it=node.offsets.begin();
       it!=node.offsets.end(); it++) {
    trace_t i = *it;
    for (trace_t j=0; j<plen; j++) {
      *(text+i+j) = maxID;
    }
  }
  return text;
}

alpha_t *
SSTree::zeroOffsetsInSeq(alpha_t *text, list<trace_t> offsets)
{
  alpha_t special = (1 << (8*sizeof(alpha_t))) - 2;
  for (list<trace_t>::iterator it=offsets.begin();
       it!=offsets.end(); it++) {
    trace_t i = *it;
    *(text+i) = special;
  }
  return text;
}



void
SSTree::PrintAnnRect(trace_t x, trace_t len, string label)
{
  cout << "r " << x << " 0 " << len << " max 0 1 0 0.15 " << label << endl;
}

void
SSTree::PrintAnnOfNode(MyNode n)
{
  stringstream ss;
  ss << n.id;

  for (list<trace_t>::iterator it=n.offsets.begin();
       it!=n.offsets.end(); it++) {
    PrintAnnRect(*it, n.edgeLen, ss.str());
  }
}

void
SSTree::PrintAnnOfNodePath(MyNode n)
{
  stringstream ss;
  ss << n.id;

  trace_t plen = GetPathLen(n.id);
  trace_t diff = plen - n.edgeLen;
  for (list<trace_t>::iterator it=n.offsets.begin();
       it!=n.offsets.end(); it++) {
    PrintAnnRect(((*it) - diff), plen, ss.str());
  }
}

void
SSTree::PrintAnnVline(trace_t x, string label)
{
  cout << "v " << x << " 1 0 0 0.15 " << label << endl;
}

void
SSTree::PrintAnnOfOffsets(list<trace_t> offsets, string label)
{
  for (list<trace_t>::iterator it=offsets.begin();
       it!=offsets.end(); it++) {
    //PrintAnnVline(*it, "");
    PrintAnnRect(*it, 1, "");
  }
}

/**
 * Returns the Right most leaf of the (internal) node v.
 */
ulong SSTree::rightmost(ulong v)
{
  return brLeaf->select(brLeaf->rank(Pr->findclose(v)));
}

/**
 * Returns the Left most leaf of the (internal) node v.
 */
ulong SSTree::leftmost(ulong v)
{
  return brLeaf->select(brLeaf->rank(v)+1);
}

/**
 * Returns the Left rank of a (leaf) node.
 *
 * @see textpos()
 */
ulong SSTree::leftrank(ulong v)
{
  return brLeaf->rank(v - 1);
}

/**
 * Returns the Inorder number of a node
 */
ulong SSTree::inorder(ulong v)
{
  return brLeaf->rank(Pr->findclose(v+1)) - 1;
}

/**
 * Returns the Number of nodes in a subtree rooted at the node v.
 */
ulong SSTree::numberofnodes(ulong v)
{
  return (Pr->findclose(v)-v-1)/2+1;
}

/**
 * Returns the Number of leaf nodes in a subtree rooted at the node v.
 */
ulong SSTree::numberofleaves(ulong v)
{
  return leftrank(Pr->findclose(v))-leftrank(v);
}

/**
 * Returns the Suffix array value of the leaf node v.
 */
ulong SSTree::textpos(ulong v)
{
  // works correctly if v is a leaf
  // otherwise returns the textpos of leaf previous to v in preorder  
  return sa->lookup(this->leftrank(v));
}

/**
 * Check for an open parentheses at index v.
 */
ulong SSTree::isOpen(ulong v)
{
  return Pr->isOpen(v);
}

/**
 * Search for a given pattern of length l from the suffix array.
 * Returns a node from the suffix tree that is LCA of the matching leaf interval.
 */
ulong SSTree::search(alpha_t *pattern, ulong l)
{
  if (l == 0)
    return 0;
        
  ulong sp, ep;
  if (!sa->Search(pattern, l, &sp, &ep))
    return root();       // Return empty match
    
  // Fetch leaf nodes
  sp = brLeaf->select(sp+1);
  ep = brLeaf->select(ep+1);

  // Check if sp and ep are the same leaf node
  if (sp == ep)
    return sp;
  ep ++;

  // Get rank_1's
  ulong r_sp = br->rank(sp),
    r_ep = br->rank(ep);
  // Calculate number of open and closed parentheses on the interval [sp, ep]
  ulong open = r_ep - r_sp + 1,
    close = (ep - r_ep) - (sp - r_sp);
    
  // Correct boundaries
  if (open < close) 
    {
      sp   -= close - open;
      r_sp -= close - open;  // Rank changes also
    }
  if (open > close)
    ep += open - close;    // Rank (r_ep) doesn't change
    
  // Number of close parentheses on the interval [0, sp]
  close = sp - r_sp + 1;
  // Index of the nearest closed parenthesis to the left from the index sp
  if (close > 0)
    close = br->select0(close);
  else
    close = 0;  // Safe choice when there is no closed parenthesis at left side

  // Number of 1-bits in the balanced parentheses sequence --- the last bit is always 0
  open = br->rank(br->NumberOfBits() - 2);
  // Index of the nearest open parenthesis to the right from the index ep
  if (r_ep + 1 <= open)
    open = br->select(r_ep + 1);
  else
    open = br->NumberOfBits() - 1; // Safe choice when there is no open parenthesis at right side
    
  // Select the closest of these two limits
  if (sp - close <= open - ep)
    return close + 1;
  else
    return sp - (open - ep) + 1;
}

