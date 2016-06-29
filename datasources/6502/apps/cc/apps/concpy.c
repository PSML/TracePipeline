#define CONMEM ((char *)0xFF00)

int
main(void)
{
  char c;

  while (1) {
    c = *CONMEM;
    if (c == 0) break;
    *CONMEM = c;
  }

  return 0;
}
