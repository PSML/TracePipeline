#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"

// use globals to avoid stack and ensure simpler
// assembler output
int8_t a8 = 0xFF;
int8_t b8 = 0x01;
int8_t c8;

uint8_t ua8 = 0xFF;
uint8_t ub8 = 0x01;
uint8_t uc8;

int32_t a32 = 0x00003333;
int32_t b32 = 0x00004444;
int32_t c32;

uint32_t ua32 = 0x00003333;
uint32_t ub32 = 0x00004444;
uint32_t uc32; //0x00010000;

void
uint32_loop(void)
{
  for (ua32=1; ua32<2; ua32++) {
    cprintf("uint32_loop: ua=0x%" PRIx32 " (%" PRId32 ")\n",
	    ua32, ua32);
  }
}
void 
int8_add(void)
{
  c8 = a8 + b8;
}

void
int8_sub(void)
{
  c8 = a8 - b8;
}

void
uint8_add(void)
{
  uc8 = ua8 + ub8;
}

void
uint8_sub(void)
{
  uc8 = ua8 - ub8;
}

void
uint8_mult3(void)
{
  uc32 = ua8 * 3;
}

void
int32_add(void)
{
  c32 = a32 + b32;
}

void
int32_sub(void)
{
  c32 = a32 - b32;
}

void 
uint32_add(void)
{
  uc32 = ua32 + ub32;
}

void 
uint32_sub(void)
{
  uc32 = ua32 - ub32;
}

extern void
uint32_div2(void)
{
  uc32 = ua32 / 2;
}

int main(void)
{
#if 0
  cprintf("sizeof(char)=%" PRIuMAX "sizeof(int)=%" PRIuMAX " sizeof(long)=%" 
	  PRIuMAX "\n", sizeof(char), sizeof(int), sizeof(long));

  int8_add();
  cprintf("int_add a=%" PRId8 " b=%" PRId8 " c=%" PRId8"\n", 
	  a8, b8, c8);

  int8_sub();
  cprintf("int8_sub a=%" PRId8 " b=%" PRId8 " c=%" PRId8 "\n", 
	  a8, b8, c8);

  uint8_add();
  cprintf("uint8_add ua=0x%" PRIx8 " ub=0x%" PRIx8 " uc=0x%" PRIx8 "\n", 
	  ua8, ub8, uc8);

  uint8_sub();
  cprintf("uint8_sub ua=0x%" PRIx8 " ub=0x%" PRIx8 " uc=0x%" PRIx8 "\n", 
	  ua8, ub8, uc8);

  uint8_mult3();
  cprintf("uint8_mult3 ua=0x%" PRIx8 " (%" PRId8 ") uc=0x%" PRIx32 " (%" PRId32 ")\n", 
	  ua8, ua8, uc32, uc32);

  int32_add();
  cprintf("int32_add a=0x%" PRIx32 " b=0x%" PRIx32 " c=0x%" PRIx32 "\n", a32, b32, c32);

  int32_sub();
  cprintf("int32_sub a=0x%" PRIx32 " b=0x%" PRIx32 " c=0x%" PRIx32 "\n", a32, b32, c32);

  uint32_add();
  cprintf("uint32_add ua=0x%" PRIx32 " ub=0x%" PRIx32 " uc=0x%" PRIx32 " (%" 
	  PRId32 ")\n", ua32, ub32, uc32, uc32);

  uint32_sub();
  cprintf("uint32_sub ua=0x%" PRIx32 " ub=0x%" PRIx32 " uc=0x%" PRIx32 " (%" 
	  PRId32 ")\n", ua32, ub32, uc32, uc32);

  uint32_div2();
  cprintf("uint32_div ua=0x%" PRIx32 " (%" PRId32 ") uc=0x%" PRIx32 " (%" PRId32 ")\n",
          ua32, ua32, uc32, uc32);
#else
  uint32_loop();
#endif

  return 0;
}
