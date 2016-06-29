#include <xmmintrin.h>
#include <stdint.h>
#include <string.h>

struct IOsvnums {
  int svnums[100];
  int cnt;
  int idx;
};

int instnum = 0;
unsigned char *init;
struct IOsvnums ios;

typedef __m128i vector; // vector of 4 int   (mmx)

inline void
bitxor_vdiff(vector *a, vector *b, vector *c)
{
  *c = *a ^ *b; 
}

union vec128 {
  vector   v;
  struct {
    uint64_t a;
    uint64_t b;
  } data;
};

int
bitxorseg_buf(unsigned char *ibuf, unsigned char *obuf,
	     int size, int bsize)
{
  int numblocks = size/bsize;
  int numvec = bsize/sizeof(vector);
  union vec128 *ba, *bb, *bc;
  union vec128 va __attribute__ ((aligned(16)));
  union vec128 vb __attribute__ ((aligned(16)));
  union vec128 vc __attribute__ ((aligned(16)));
  int i,j;

  for (i=0; i<numblocks; i++) {
    instnum++;
    if (ios.idx <= ios.cnt && instnum == ios.svnums[ios.idx]) {
      memcpy(init, &ibuf[i*bsize], bsize);
      ios.idx++;
    }
    ba=(union vec128 *)init;
    bb=(union vec128 *)&ibuf[i*bsize];
    bc=(union vec128 *)&obuf[i*bsize];
    for (j=0; j<numvec; j++) {
      va.data.a = ba[j].data.a;
      va.data.b = ba[j].data.b;
      vb.data.a = bb[j].data.a;
      vb.data.b = bb[j].data.b;
      bitxor_vdiff(&(va.v), &(vb.v), &(vc.v));
      bc[j].data.a = vc.data.a;
      bc[j].data.b = vc.data.b;
    }
    // take care of excess bytes that are not taken care of by vector
    // ops -- could get fancy but lets just do byte ops for the moment
    // could have done 64s, 32s, 16s, and then 8s
    unsigned char *ca, *cb, *cc;
    ca=init;
    cb=&ibuf[i*bsize];
    cc=&obuf[i*bsize];
    for (j=numvec * sizeof(vector); j<bsize; j++) {
      cc[j] = ca[j] ^ cb[j]; 
    }
  }
  return numblocks * bsize;
}

#ifdef __BITXORSEG_STAND_ALONE__
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_NUMBLOCKS 1024

/* USAGE: bitxorseg <blocksize> <io file> */

int 
main(int argc, char **argv)
{
  int            n, p, rc, blocksize, bufsize;
  FILE          *fp;
  unsigned char *ibuf, *obuf;
  
  blocksize = atoi(argv[1]);
  bufsize   = blocksize * BUF_NUMBLOCKS;

  if (!(fp = fopen(argv[2], "r"))) {
    perror("ERROR: fopen");
    return -1;
  }

  ios.cnt = 0;
  ios.idx = 0;
  while (fscanf(fp, "%d", &ios.svnums[ios.cnt]) != EOF) ios.cnt++;

  if (posix_memalign((void *)&ibuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&obuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&init, 4096, blocksize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }
  
  if (freopen(NULL, "rb", stdin) == 0) {
    perror("ERROR: freopen");
    return -1;
  }

  if ((n = fread(init, 1, blocksize, stdin)) < blocksize) {
    fprintf(stderr, "ERROR: n=%d fread less than blocksize=%d\n",
	    n, blocksize);
    return -1;
  }

  while ((n = fread(ibuf, 1, bufsize, stdin)) > 0) {
    if (n % blocksize) {
      fprintf(stderr, "ERROR: n=%d fread not a multiple of blocksize=%d\n", 
	      n, blocksize);
      return -1;
    }

    rc = bitxorseg_buf(ibuf, obuf, n, blocksize);
    if (rc != n) {
      fprintf(stderr, "n=%d but only did %d bytes (blocksize=%d)\n",
	      n, rc, blocksize);
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
#endif
