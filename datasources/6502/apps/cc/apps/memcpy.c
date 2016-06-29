#if STR == 1
char src[12] = "JoNaThaN123"; // str 1
#elif STR == 2
char src[12] = "012TOMMy210"; // str 2
#elif STR == 3
char src[12] = "E+uR,F(J-nk"; // str 3
#else
char src[12] = "Hello World";
#endif

char dst[12];

int
main(void)
{
  char i=0;

  while (1) {
    dst[i] = src[i];
    if (src[i] == 0) break;
    i++;
  }

  return 0;
}
