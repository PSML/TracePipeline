#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_NUMBLOCKS 1024

int
bitset_buf(unsigned char *ibuf, unsigned char *obuf,
	    int size, int bsize)
{
  int numblocks = size/bsize;
  int i,j,k,tmp,rc=0;
  unsigned char c;

  for (i=0; i<numblocks; i++) {
    for (j=0; j<bsize; j++) {
      if ((c = ibuf[i*bsize+j])) {
	/* test each bit */
	for (k=0; k<8; k++) {
	  if (c & (0x01<<k)) {
	    if ((tmp = sprintf((char *)obuf+rc, "%d ", j*8+k)) <= 0) {
	      perror("sprintf");
	      return -1;
	    }
	    rc += tmp;
	  }
	}
      }
    }
    if ( (tmp = sprintf((char *)obuf+rc, "\n")) <= 0 ) {
      perror("sprintf");
      return -1;
    }
    rc += tmp;
  }

  return rc;
}

int 
main(int argc, char **argv)
{
  int            n, p, rc, blocksize, bufsize;
  unsigned char *ibuf, *obuf;
  
  blocksize = atoi(argv[1]);
  bufsize   = blocksize * BUF_NUMBLOCKS;
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }
  if (posix_memalign((void *)&obuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (freopen(NULL, "rb", stdin) == 0) {
    perror("ERROR: freopen");
    return -1;
  }

  while ((n = fread(ibuf, 1, bufsize, stdin)) > 0) {
    if (n % blocksize) {
      fprintf(stderr, "ERROR: n=%d read not a multiple of blocksize=%d\n", 
	      n, blocksize);
    }

    rc = bitset_buf(ibuf, obuf, n, blocksize);
    if (rc < 0) {
      perror("bitset");
      return -1;
    }

    p = fwrite(obuf, 1, rc, stdout);
    if (p != rc) {
      perror("write");
      return -1;
    }
  }

  fclose(stdin);

  return 0;
}
