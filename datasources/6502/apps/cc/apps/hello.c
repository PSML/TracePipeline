#include <conio.h>

char str[80]="My string.";

int main(void)
{ 
  char c = 'a';
  int  i = 45;

  cprintf("Hello World!!!\n");
  cprintf("c=%c\n", c);
  cprintf("i=%d (%x)\n", i, i);
  cprintf("str=%s\n", str);
  cprintf("sizeof(int)=%d sizeof(long)=%d sizeof(unsigned long)=%d\n", 
	  sizeof(int), sizeof(long), sizeof(unsigned long));

  return 0;   
}
