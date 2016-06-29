
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "SSTree.h"
#include "Tools.h"
/*
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
*/
using namespace std;

void
PrintAccuracy(SSTree *sst, trace_t n, int c)
{
  for (trace_t i=1; i<n-1; i++) {
    cout << i << " ";
    sst->PrintAccuracy(i, c);
  }
}

void
printUsage()
{
  cerr << "-o   set ofile (default stdout)" << endl;
  cerr << "-n   set nodes (default root)" << endl;
  cerr << "-d   set depth (default max)" << endl;
  cerr << "-p   print parent" << endl;
  cerr << "-m   call merging algorithm" << endl;
  cerr << "-N   PrintNodes to ofile of nodes" << endl;
  cerr << "-T   PrintMyTree to ofile from nodes to depth" << endl;
  cerr << "-S   PrintSuffixesFrom to ofile of nodes" << endl;
  cerr << "-H   PrintHotPaths to ofile of nodes" << endl;
  cerr << "-A   PrintCausalSyms to ofile of nodes" <<endl;
  cerr << "-C   PrintNodeEdgeCount to ofile from nodes to depth" << endl;
  cerr << "-E   PrintEdge symbols to ofile of nodes" << endl;
  cerr << "-F   PrintOffsets to ofile of nodes" << endl;
  cerr << "-I   PrintNodeInfo to ofile of nodes" << endl;
  cerr << "-q   quit" << endl;
}

void
doDotHeader()
{
  cout << "strict digraph G {" << endl;
  cout << "node [shape=plaintext]" << endl;
}

SSTree *
createSST(alpha_t *seq, trace_t n, char *hstr, bool first)
{
  cerr << "\nCreating Sadakane's suffix tree...\n";
  
  SSTree *sst;
  if (hstr == NULL) {
    sst = new SSTree(seq, n, false, 0, SSTree::nop, 0);
  } else {
    sst = new SSTree(seq, n, false, 0, SSTree::nop, 0, (const char *)hstr);
  }
  if (first) {
    cerr << "READY:\n";
  } else {
    cerr << "READY again:\n";
  }
  return sst;
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "USAGE: " << argv[0] << " <seq file> [htmlHostString]" << endl;
    return -1;
  }
  char *seqfile = argv[1];
  trace_t n=0;
  alpha_t *seq = Tools::GetFileContents(seqfile, &n);
  // add terminating symbol 0xFFFF at the end of sequence
  *(seq+n) = (1 << (8*sizeof(alpha_t))) - 1; n++;
  // takes the end symbol 0u as part of the sequence
  *(seq+n) = 0; n++; 

  char *hstr=0;
  if (argc >= 3 ) hstr = argv[2];

  SSTree *sst = createSST(seq, n, hstr, true);
  

  /*
  ofstream ofs("savedsst.txt");
  {
    boost::archive::text_oarchive oa(ofs);
    oa & sst;
  }
  */

  int quit=0;
  while (!quit) {
    char commandLine[4096];
    cin.getline(commandLine, 4096);

    char *av[4096];
    int ac=1; optind=1;
#ifdef __APPLE__
    optreset=1;
#endif
    char *p = strtok(commandLine, " ");
    while (p) {
      av[ac] = p; ac++;
      p = strtok(0, " ");
    }

    int dotHeader=0;
    ofstream out;
    streambuf *coutbuf;
    char *ofile=0, *nodes=0, *symbols=0;
    list<trace_t> nlist;
    list<alpha_t> slist;
    list<trace_t> clist;

    int depth=0, parent=0, merge=0, text=0;

    int opt;
    char optParsed = false;
    while ((opt = getopt(ac, av, "NPTHASCEFIo:n:s:d:pmtq")) != -1) {
      optParsed = true;
      int pMyTree=0, pHotPath=0, pCausalSyms=0;
      int pNode=0, pNodeInfo=0, pTreeInfo=0, pPath=0;
      int pNodeEdgeCnt=0, pNodeEdge=0, pNodeSuffix=0, pOffsets=0, doOutput=0;
      nodes=0; symbols=0;

      switch (opt) {
	// Caps cause output
      case 'N':
        pNode = 1; doOutput=1; break;
      case 'P':
        pPath = 1; doOutput=1; break;
      case 'T':
	pMyTree = 1; doOutput=1; break;
      case 'H':
	pHotPath = 1; doOutput=1; break;
      case 'A':
	pCausalSyms = 1; doOutput=1; break;
      case 'S':
	pNodeSuffix = 1; doOutput=1; break;
      case 'C':
	pNodeEdgeCnt = 1; doOutput=1; break;
      case 'E':
	pNodeEdge = 1; doOutput=1; break;
      case 'F':
	pOffsets = 1; doOutput=1; break;
      case 'I':
	pNodeInfo = 1; doOutput=1; break;
      case 'O':
	pTreeInfo = 1; doOutput=1; break;
	// Lower Case are modifiers to the print
      case 'o':
	ofile = optarg; break;
      case 'n':
	nodes = optarg; break;
      case 's':
	symbols = optarg; break;
      case 'd':
	depth = stoi(optarg); break;
      case 'p':
	parent = 1; break;
      case 'm':
	merge = 1; break;
      case 't':
	text = 1; break;
      case 'q':
	quit = 1; break;
      default:
	printUsage(); break;
      }
      if (nodes) {
	char *token = strtok(nodes, ",");
	while (token) {
	  nlist.push_back(stoi(token));
	  token = strtok(0, ",");
	}
      }
      if (symbols) {
	char *token = strtok(symbols, ",");
	while (token) {
	  slist.push_back(stoi(token));
	  token = strtok(0, ",");
	}
      }
      if (merge) {
	// get LCP of read 0xff00 symbols
	//	list<alpha_t> syms = sst->GetLCPEndingIn(slist);
	//	trace_t v = sst->GetNodeOfSymbols(syms);

	// replace all occurences of node v with a new symbol in seq
	//	seq = sst->addSymToSeq(seq, n, v);
	// recreate sst
	//sst = createSST(seq, n, hstr);

	// get the children of all the node starting with new symbol

	// for each level of v, cluster internal nodes based on edgeLen
	list<list<MyNode>> clusters;
	list<list<trace_t>> rOffsets;
	sst->clusterChildNodesOf(298932, clusters, rOffsets);

	// for (list<list<MyNode>>::iterator cIts=clusters.begin();
	//      cIts!=clusters.end(); cIts++) {
	//   list<MyNode> cluster = *cIts;
	//   list<trace_t> offsets = rOffsets.front(); rOffsets.pop_front();
	//   getOffsetsFromCluster(cluster, offsets);
	// }

	coutbuf = cout.rdbuf(); // save old buf
	if (ofile) {
	  out.open(ofile);
	  cout.rdbuf(out.rdbuf()); // redirect cout
	}

	list<trace_t> offsets = sst->getOffsetsFromCluster(clusters.front(), rOffsets.front());

#if 0
	list<MyNode> tmp = clusters.front();
	for (list<MyNode>::iterator it=tmp.begin();
	     it!=tmp.end(); it++) {
	  sst->PrintAnnOfNode(*it);
	}
	sst->PrintAnnOfOffsets(offsets, "");
#endif
	
	// replace symbols at the offsets with a special symbol
	seq = sst->zeroOffsetsInSeq(seq, offsets);
	// recreate sst
	sst = createSST(seq, n, hstr, false);

	// find d1 node of special symbol
	alpha_t special = (1 << (8*sizeof(alpha_t))) - 2;
	trace_t v = sst->GetChildNodeOfSymbol(sst->root(), special);

	cerr << v << endl;

	// for each leaf, 1) find hot path, 2) identify prefix and blackbox
	
      }
      if (doOutput) {
	coutbuf = cout.rdbuf(); // save old buf
	if (ofile) {
	  out.open(ofile);
	  cout.rdbuf(out.rdbuf()); // redirect cout
	}
	if (pNode) {
	  if (!dotHeader) { doDotHeader(); dotHeader=1; }
	  for (list<trace_t>::iterator it=nlist.begin();
	       it!=nlist.end(); it++) {
	    trace_t v = *it;
	    sst->PrintDotNode(v);
	    if (parent) {
	      trace_t p = sst->parent(v);
	      sst->PrintDotNode(p);
	      sst->PrintDotEdge(p, v);
	    }
	  }
	  for (list<alpha_t>::iterator it=slist.begin();
	       it!=slist.end(); it++) {
	    trace_t v = sst->GetChildNodeOfSymbol(sst->root(), *it);
	    sst->PrintDotNode(v);
	    if (parent) {
	      sst->PrintDotNode(sst->root());
	      sst->PrintDotEdge(sst->root(), v);
	    }
	  }
	}
	else if (pPath) {
	  sst->PrintLCPEndingIn(slist, 1);
	}
	else if (pMyTree) {
	  if (!dotHeader) { doDotHeader(); dotHeader=1; }
	  for (list<trace_t>::iterator it=nlist.begin();
	       it!=nlist.end(); it++) {
	    sst->PrintMyTree(*it, depth);
	  }
	}
	else if (pNodeSuffix) {
	  if (!dotHeader) { doDotHeader(); dotHeader=1; }
	  sst->PrintSuffixesFrom(nlist);
	}
	else if (pHotPath) {
	  if (!dotHeader) { doDotHeader(); dotHeader=1; }
	  for (list<trace_t>::iterator it=nlist.begin();
	       it!=nlist.end(); it++) {
	    sst->PrintHotPath(*it);
	  }
	}
	else if (pCausalSyms) {
	  sst->PrintCausalSyms(nlist, 0);
	}
	else if (pNodeEdgeCnt) {
	  trace_t node;
	  list<trace_t>::iterator it;
	  for (it=nlist.begin(); it!=nlist.end(); it++) {
	    node = *it;
	    sst->PrintNodeEdgeCount(node, depth);
	  }
	}
	else if (pNodeEdge) {
	  list<trace_t>::iterator it;
	  for (it=nlist.begin(); it!=nlist.end(); it++) {
	    sst->PrintEdge(*it);
	  }
	}
	else if (pOffsets) {
	  sst->PrintPathOffsets(nlist, depth, 1, 1);
	}
	else if (pNodeInfo) {
	  trace_t node;
	  list<trace_t>::iterator it;
	  for (it=nlist.begin(); it!=nlist.end(); it++) {
	    node = *it;
	    sst->PrintNodeInfo(node);
	  }
	}
	else if (pTreeInfo) {
	  sst->PrintTreeInfo();
	}
	doOutput=0; depth=0; parent=0; merge=0;
	nlist.clear(); slist.clear();
      }
    }
    if (!optParsed) {
      printUsage();
      continue;    
    }
    if (dotHeader) cout << "}" << endl;
    if (ofile) {
      cout.rdbuf(coutbuf); // reset to stdout
      out.close();
    }
  }
  delete sst;
  delete [] seq;
  return 0;
}
