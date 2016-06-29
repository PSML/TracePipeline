#ifdef DEBUG
#include <stdio.h>
#endif

#ifndef SIZE
#define SIZE 10
#endif

#ifndef KEY
#define KEY "1234567890"
#endif

char key[SIZE] = KEY;

unsigned int hash, i; 

int main()
{
#ifdef DEBUG
  printf("KEY = %s\n", KEY);
#endif
  for(hash = i = 0; i < SIZE; i++)
    {
      hash += key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}
