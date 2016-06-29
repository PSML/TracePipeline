
#include "MyNode.h"

MyNode::MyNode()
{
  count     = 0;
  slinkTo   = 0;
}

MyNode::~MyNode()
{
  children.clear();
  offsets.clear();
  slinkFrom.clear();
}
