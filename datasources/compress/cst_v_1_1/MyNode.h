
#ifndef _MYNODE_H_
#define _MYNODE_H_

#include "Tools.h"
#include <list>
#include <fstream>

/*
// Forward declaration of class boost::serialization::access
namespace boost {
namespace serialization {
class access;
}
}
*/

class MyNode {
 public:
  trace_t id;
  trace_t edgeLen;  
  trace_t count;
  trace_t slinkTo;
  std::list<trace_t> slinkFrom;
  std::list<trace_t> offsets;
  trace_t parent;
  std::list<trace_t> children;
  MyNode();
  ~MyNode();
  
 /*
 private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    // Simply serialize the data members of MyNode
    ar & edgeLen;
    ar & count;
    ar & slinkTo;
    ar & slinkFrom;
    ar & parent;
    ar & children;
    ar & offsets;
  }
 */
};

#endif
