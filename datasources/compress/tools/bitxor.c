#include <xmmintrin.h>
#include <stdint.h>

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
bitxor_buf(unsigned char *ibuf, unsigned char *obuf,
	  int size, int bsize)
{
  int numblocks = size/bsize;
  int numvec = bsize/sizeof(vector);
  union vec128 *ba, *bb, *bc;
  union vec128 va __attribute__ ((aligned(16)));
  union vec128 vb __attribute__ ((aligned(16)));
  union vec128 vc __attribute__ ((aligned(16)));
  int i,j;

  for (i=0; i<(numblocks-1); i++) {
    ba=(union vec128 *)&ibuf[i*bsize];
    bb=(union vec128 *)&ibuf[(i+1)*bsize];
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
    ca=&ibuf[i*bsize];
    cb=&ibuf[(i+1)*bsize];
    cc=&obuf[i*bsize];
    for (j=numvec * sizeof(vector); j<bsize; j++) {
      cc[j] = ca[j] ^ cb[j]; 
    }
  }
  return (numblocks-1) * bsize;
}

#ifdef __BITXOR_STAND_ALONE__
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include "../uthash/src/uthash.h"

/* USAGE: bitxor -b <blocksize> -f <filename> */

#define BUF_NUMBLOCKS 1024

int ID  = 0;
int FID = 0;

typedef struct {
  long xor;
  int  cnt;
  int  id;
  int  fid;
  UT_hash_handle hh;
} xorhash_t;
xorhash_t *HASH = NULL;

long
sha(unsigned char *xor, int blocksize)
{
  unsigned char buf[32];

  /* Compute SHA-256.  */
  SHA256(xor, blocksize, buf);

  /* Build integer from low bytes of SHA-256.  */
  return (buf[0] << 8) | buf[1];
}

int
hash_xor(unsigned char *ibuf, int bufsize, int blocksize)
{
  int i=0;
  long xorhash;
  xorhash_t *h;
  unsigned char *xor = (unsigned char *)malloc(blocksize);

  while (i < bufsize) {
    memcpy(xor, ibuf+i, blocksize);
    xorhash = sha(xor, blocksize);

    HASH_FIND(hh, HASH, &xorhash, sizeof(long), h);
    if (h) {
      h->cnt++;
    } else {
      h = (xorhash_t *)malloc(sizeof(xorhash_t));
      h->xor = xorhash;
      h->cnt = 1;
      h->id = ID;
      ID++;
      HASH_ADD(hh, HASH, xor, sizeof(long), h);
    }
    i += blocksize;
  }
  return i;
}

int
sort_xorhash_cnt(xorhash_t *a, xorhash_t *b)
{
  return (b->cnt - a->cnt);
}

void
init_fid()
{
  xorhash_t *h;

  HASH_SORT(HASH, sort_xorhash_cnt);
  for (h=HASH; h!= NULL; h=h->hh.next) {
    h->fid = FID;
    FID++;
  }
}

int
print_fid(unsigned char *ibuf, int bufsize, int blocksize)
{
  int i=0;
  long xorhash;
  xorhash_t *h;
  unsigned char *xor = (unsigned char *)malloc(blocksize);

  while (i < bufsize) {
    memcpy(xor, ibuf+i, blocksize);
    xorhash = sha(xor, blocksize);

    HASH_FIND(hh, HASH, &xorhash, sizeof(long), h);
    if (h) {
      fprintf(stdout, "%d\n", h->fid);
    } else {
      fprintf(stderr, "ERROR: not in hash\n");
      return -1;
    }
    i += blocksize;
  }
  return i;
}

int 
main(int argc, char **argv)
{
  int            n, p, rc, blocksize=0, bufsize, bufsize_read;
  unsigned char *ibuf, *obuf, *ibuf_read;
  int            i, c, mflag=0;
  char          *filename=0;
  FILE          *fp;

  while ((c = getopt(argc, argv, "b:f:m")) != -1) {
    switch (c) {
    case 'b':
      if (sscanf(optarg, "%d", &blocksize) != 1) {
	fprintf(stderr, "illegal argument pair: '-b %s'", optarg);
	return -1;
      }
      break;
    case 'f':
      filename = (char *)malloc(strlen(optarg));
      if (strcpy(filename, optarg) == 0) {
	fprintf(stderr, "ERROR: strcpy");
	return -1;
      }
      break;
    case 'm':
      mflag = 1;
      break;
    }
  }
  
  if (blocksize == 0) return -1;
  bufsize = blocksize * BUF_NUMBLOCKS;
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&obuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }
  
  if (filename) {
    if ((fp = fopen(filename, "r")) == 0) {
      perror("ERROR: fopen");
      return -1;
    }
  } else {
    if ((fp = freopen(NULL, "rb", stdin)) == 0) {
      perror("ERROR: freopen");
      return -1;
    }
  }

  ibuf_read = ibuf;
  bufsize_read = bufsize;
  while ((n = fread(ibuf_read, 1, bufsize_read, fp)) > 0) {
    if (n % blocksize) {
      fprintf(stderr, "ERROR: n=%d fread not a multiple of blocksize=%d\n", 
	      n, blocksize);
    }
    if (ibuf != ibuf_read) n += blocksize;

    rc = bitxor_buf(ibuf, obuf, n, blocksize);
    if (rc != n-blocksize) {
      fprintf(stderr, "n=%d but only did %d bytes (blocksize=%d)\n",
	      n, rc, blocksize);
    }

    if (mflag) {
      hash_xor(obuf, rc, blocksize);
    } else {
      p = fwrite(obuf, 1, rc, stdout);
      if (p != rc) {
	perror("write");
	return -1;
      }
    }
    /* copy over the last sv for bitxor with the next block */
    for (i=0; i<blocksize; i++){
      ibuf[i] = ibuf[n-blocksize+i];
    }
    ibuf_read = ibuf + blocksize;
    bufsize_read = bufsize - blocksize;
  }

  if (mflag) {
    init_fid();

    fseek(fp, 0, SEEK_SET);

    ibuf_read = ibuf;
    bufsize_read = bufsize;
    while ((n = fread(ibuf_read, 1, bufsize_read, fp)) > 0) {
      if (n % blocksize) {
	fprintf(stderr, "ERROR: n=%d fread not a multiple of blocksize=%d\n",
		n, blocksize);
      }
      if (ibuf != ibuf_read) n += blocksize;

      rc = bitxor_buf(ibuf, obuf, n, blocksize);
      if (rc != n-blocksize) {
	fprintf(stderr, "n=%d but only did %d bytes (blocksize=%d)\n",
		n, rc, blocksize);
      }
      print_fid(obuf, rc, blocksize);

      /* copy over the last sv for bitxor with the next block */
      for (i=0; i<blocksize; i++){
	ibuf[i] = ibuf[n-blocksize+i];
      }
      ibuf_read = ibuf + blocksize;
      bufsize_read = bufsize - blocksize;
    }
  }

  fclose(fp);

  return 0;
}
#endif
