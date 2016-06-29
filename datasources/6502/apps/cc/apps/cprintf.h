#ifndef __CPRINTF_H__
#define __CPRINTF_H__

#ifdef __6502__
#include <conio.h>
#else
#include <stdio.h>
#define cprintf printf
#define cscanf scanf
#endif

#endif
