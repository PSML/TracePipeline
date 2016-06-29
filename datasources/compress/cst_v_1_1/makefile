CC = g++
CPPFLAGS = -Wall -ansi -pedantic -O3 -std=c++11 

objects = TestPSML.o SSTree.o Tools.o CSA.o BitRank.o \
ReplacePattern.o wtreebwt.o bittree.o rbtree.o CHgtArray.o CRMQ.o \
SubblockRMQ.o Parentheses.o Hash.o LcpToParentheses.o MyNode.o

default: $(objects)
	$(CC) -o TestPSML $(objects)

clean:
	rm -f core *.o *~ TestLCSS TestPSML 

depend:
	g++ -MM *.cpp > dependencies.mk

include dependencies.mk
