
int
main(void)
{
  char *c = (char *)0xFF00;
  *c = 'a';

  *((char *)0xFF00) = 'b';

  return 0;
}
