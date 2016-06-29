/*
 * Collection of basic tools and defines
 */

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <iostream>
#include <fstream>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <cstdint>

#include <iomanip>

#define __WORDSIZE 64
// Generates an error if __WORDSIZE is not defined
#ifndef __WORDSIZE
#error Missing definition of __WORDSIZE; Please define __WORDSIZE in Tools.h!
#endif

// Check word length on GNU C/C++:
// __WORDSIZE should be defined in <bits/wordsize.h>, which is #included from <limits.h>
#if __WORDSIZE == 64
#   define W 64
#else
#   define W 32
#endif

#define WW (W*2)
#define Wminusone (W-1)

#ifndef alpha_t
#define alpha_t uint16_t
#endif
#ifndef salpha_t
#define salpha_t int32_t
#endif
#ifndef trace_t
#define trace_t uint64_t
#endif

#ifndef ALPHABETSIZE
#define ALPHABETSIZE 65536
#endif

#ifndef ulong
#define ulong uint64_t
#endif

class Tools
{
private:
static time_t startTime;
public:
static void StartTimer();
static double GetTime();
static alpha_t * GetRandomString(unsigned, unsigned, unsigned &);
static void PrintBitSequence(ulong *, ulong);
static alpha_t * GetFileContents(char *, trace_t *);
static ulong ustrlen(alpha_t *);
static char *uctoc(alpha_t *, bool = false);
static alpha_t *ctouc(char *, bool = false);
static void RemoveControlCharacters(alpha_t *);
static ulong * bp2bitstream(alpha_t *);
static unsigned FloorLog2(ulong);
static unsigned CeilLog2(ulong);
static unsigned bits (ulong);
static unsigned* MakeTable();
static unsigned FastFloorLog2(unsigned);

static inline void SetField(ulong *A, register unsigned len, register ulong index, register ulong x) 
{
ulong i = index * len / W;
ulong j = index * len - i * W;
ulong mask = (j+len < W ? ~0lu << (j+len) : 0) 
  | (W-j < W ? ~0lu >> (W-j) : 0);
A[i] = (A[i] & mask) | x << j;
if (j + len > W) {
mask = ((~0lu) << (len + j - W));  
A[i+1] = (A[i+1] & mask)| x >> (W - j);
}
}

static inline ulong GetField(ulong *A, register unsigned len, register ulong index) 
{
register ulong i = index * len / W, 
  j = index * len - W * i, 
  result;
if (j + len <= W)
  result = (A[i] << (W - j - len)) >> (W - len);
 else 
   {
result = A[i] >> j;
result = result | (A[i+1] << (WW - j - len)) >> (W - len);
}
return result;
}
       
    
static inline ulong GetVariableField(ulong *A, register unsigned len, register ulong index) 
{
register ulong i=index/W, j=index-W*i, result;
if (j+len <= W)
  result = (A[i] << (W-j-len)) >> (W-len);
 else {
result = A[i] >> j;
result = result | (A[i+1] << (WW-j-len)) >> (W-len);
}
return result;
}

static inline void SetVariableField(ulong *A, register unsigned len, register ulong index, register ulong x)
{
ulong i = index/W;

ulong j = index-i*W;
ulong mask = (j+len < W ? ~0lu << (j+len) : 0)
  | (W-j < W ? ~0lu >> (W-j) : 0);
A[i] = (A[i] & mask) | x << j;
if (j+len>W) {
mask = ((~0lu) << (len+j-W));  
A[i+1] = (A[i+1] & mask)| x >> (W-j);
} 
}
};

#endif
