#include <xmmintrin.h>
#include <stdint.h>
#include <stdio.h>

typedef __m128i vector; // vector of 4 int   (mmx)


inline void
bdiff_vdiff(vector *a, vector *b, vector *c)
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

inline int
filter_AND(union vec128 *a, union vec128 *filter) 
{
  return (a->data.a & filter->data.a) || (a->data.b & filter->data.b); 
}

int
bdiff_buf(unsigned char *ibuf, unsigned char *obuf,
	  unsigned char *filter, int size,
	  int bsize)
{
  static int bnum=0;
  int numblocks = size/bsize;
  int numvec = bsize/sizeof(vector);
  union vec128 *ba, *bb, *bc, *fb;
  union vec128 va __attribute__ ((aligned(16)));
  union vec128 vb __attribute__ ((aligned(16)));
  union vec128 vc __attribute__ ((aligned(16)));
  int i,j;
  int fres=0;

  fb = (union vec128 *)&filter[0];

  for (i=0; i<(numblocks-1); i++, bnum++) {

    ba=(union vec128 *)&ibuf[i*bsize];
    bb=(union vec128 *)&ibuf[(i+1)*bsize];
    bc=(union vec128 *)&obuf[i*bsize];

    for (j=0; j<numvec; j++) {
      va.data.a = ba[j].data.a;
      va.data.b = ba[j].data.b;
      if (filter) {
	vb.data.a = fb[j].data.a;
	vb.data.b = fb[j].data.b;	
	fres += filter_AND(&va, &vb);
      } else {
	vb.data.a = bb[j].data.a;
	vb.data.b = bb[j].data.b;
	bdiff_vdiff(&(va.v), &(vb.v), &(vc.v));
	bc[j].data.a = vc.data.a;
	bc[j].data.b = vc.data.b;
      }
    }
    // take care of excess bytes that are not taken care of by vector
    // ops -- could get fancy but lets just do byte ops for the moment
    // could have done 64s, 32s, 16s, and then 8s
    unsigned char *ca, *cb, *cc;
    ca=&ibuf[i*bsize];
    if (filter) {
      cb=filter;
      for (j=numvec * sizeof(vector); j<bsize; j++) {
	fres = !(!(ca[j] & cb[j])); 
      }
    } else {
      cb=&ibuf[(i+1)*bsize];
      cc=&obuf[i*bsize];
      for (j=numvec * sizeof(vector); j<bsize; j++) {
	cc[j] = ca[j] ^ cb[j]; 
      }
    }

    if (fres) printf("%d\n", bnum);
  }

  return (numblocks-1) * bsize;
}


#ifdef __BDIFF_STAND_ALONE__
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_NUMBLOCKS (1024)

void
usage()
{
  fprintf(stderr,
	  "bdiff [-f <filterfile>] <blocksize> <input file> [<output file>]\n");
}

int 
main(int argc, char **argv)
{
  int            ifd, ofd, ffd;
  int            n, p, rc, blocksize, bufsize, bufsize_read;
  char          *ifile, *ofile, *ffile=NULL; 
  unsigned char *ibuf, *obuf, *ibuf_read, *fbuf;
  int            i;
  int            opt; 


  while ((opt = getopt(argc, argv, "f")) != -1) {
    switch (opt) {
    case 'f':
      ffile=optarg; break;
    default:
      usage();
      return -1;
    }
  }

  if (!ffile && optind+2 < argc) {
    usage();
    return -1;
  }
  
  blocksize = atoi(argv[optind]);
  ifile     = argv[optind+1];
  if (!ffile) ofile     = argv[optind+2];
  bufsize   = blocksize * BUF_NUMBLOCKS;
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize)!=0) {
    perror("ERROR: posix_malloc");
    return -1;
  }
  
  ifd = open(ifile, O_RDONLY);
  if (ifd < 0 ) {
    perror("ERROR: open");
    return -1;
  }

  if (!ffile) {
    if (posix_memalign((void *)&obuf, 4096, bufsize)!=0) {
      perror("ERROR: posix_malloc");
      return -1;
    }
    
    ofd = open(ofile, O_WRONLY|O_CREAT, 00666);
    if (ofd < 0 ) {
      perror("ERROR: open");
      return -1;
    }
  }

  if (ffile) {
    ffd = open(ffile, O_RDONLY);
    if (ffd < 0) {
      perror("ERROR: open");
      return -1;
    }
    if (posix_memalign((void *)&fbuf, 4096, blocksize)!=0) {
      perror("ERROR: posix_malloc");
      return -1;
    }
    if (read(ffd, fbuf, blocksize)!=blocksize) {
      perror("ERROR: filter must contain a complete block");
      return -1;
    }
    close(ffd);
  }

  ibuf_read = ibuf;
  bufsize_read = bufsize;
  while ((n = read(ifd, ibuf_read, bufsize_read)) > 0) {
    if (ibuf != ibuf_read) n += blocksize;
    if (n % blocksize) {
      fprintf(stderr, "ERROR: n=%d read not a multiple of blocksize=%d\n", 
	      n, blocksize);
    }
    rc = bdiff_buf(ibuf, obuf, fbuf, n, blocksize);
    if (rc != n-blocksize) {
      fprintf(stderr, "n=%d but only did %d bytes (blocksize=%d)\n",
	      n, rc, blocksize);
    }
    p = write(ofd, obuf, rc);  
    if (p!=rc) {
      perror("write");
      return -1;
    }
    for (i=0; i<blocksize; i++){
      ibuf[i] = ibuf[n-blocksize+i];
    }
    ibuf_read = ibuf + blocksize;
    bufsize_read = bufsize - blocksize;
  }

  return 0;
}
#endif
