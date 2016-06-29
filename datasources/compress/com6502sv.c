#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "tools/bitxor.h"
#include "uthash/src/uthash.h"

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

typedef unsigned char byte;
typedef unsigned int  id_t;

#define BLOCKSIZE     65544
#define BUF_NUMBLOCKS 1024

#define SEQ_BUFSIZE   1000000000
#define SEQ_MAXLEN    2147483647  // length is an unsigned int
                                  // minus the top bit
#define SEQ_FILEPATH  "/tmp/mmapped.bin"
#define SEQ_FILESIZE  (SEQ_BUFSIZE * sizeof(id_t))

#define MEMO_NUMINST  1024

typedef struct {
  id_t *buf;
  off_t idx;
  off_t cdx;
} seqbuf_t;

seqbuf_t SEQBUF;

typedef struct memo {
  off_t        offset;
  struct memo *next;
} memo_t;

typedef struct {
  memo_t     **tbl;
  unsigned int idx;
  unsigned int size;
} memotbl_t;

memotbl_t MEMOTBL;

typedef struct {
  byte hdr;
  byte reg[7];
  byte mem[5*3+1];
  byte rsz;
  byte msz;
} xor_t;

typedef struct {
  xor_t        xor;
  id_t         id;
  unsigned int cnt;
  UT_hash_handle hh;
} xorhash_t;

xorhash_t *XORHASH = NULL;
id_t ID = 0;

typedef struct {
  unsigned int num_uniq;
  unsigned int alphabet_size;
  unsigned int num_inst;
  unsigned int sequence_size;
} stats_t;

stats_t STATS;

xor_t *
get_xor(byte *dbuf)
{
  xor_t         *xor;
  int            i;
  byte           c;
  unsigned short addr;

  xor = (xor_t *)malloc(sizeof(xor_t));
  memset(xor, 0, sizeof(xor_t));

  for (i=0; i<BLOCKSIZE; i++) {
    /* addr = i, val = c */
    if ((c = dbuf[i])) {
      if (i == 0) {        // pc
	xor->hdr |= 1;
	xor->reg[xor->rsz++] = c;
	i++;
	xor->reg[xor->rsz++] = dbuf[i];
      } else if (i == 1) { // pc
	xor->hdr |= 1;
	xor->reg[xor->rsz++] = dbuf[i-1];
	xor->reg[xor->rsz++] = c;
      } else if (i < 7) {  // registers
	xor->hdr |= (1<<(i-1));
	xor->reg[xor->rsz++]= c;
      } else if (i > 7) {  // memory
	if (xor->msz > 4*3) {
	  fprintf(stderr, "ERROR: get_xor: msz=%d more than 5 bytes of memory changed\n", xor->msz);
	  return NULL;
	}
	if ((xor->hdr>>6) < 3) xor->hdr = (((xor->hdr>>6)+1)<<6) | (xor->hdr & 0x3f);
	addr = i-8;
	xor->mem[xor->msz++] = (byte)(0xFF & addr);        // addr low
	xor->mem[xor->msz++] = (byte)((0xFF00 & addr)>>8); // addr high
	xor->mem[xor->msz++] = c;                          // value
      }
    }
  }
  /* more than 2 bytes of memory changed */
  if (xor->msz > 2*3) {
    xor->mem[xor->msz++] = 0;
  }
  return xor;
}

int
hash_xor(byte *dbuf, unsigned int size)
{
  int        i, numblocks;
  xor_t     *xor;
  xorhash_t *h;

  numblocks = size/BLOCKSIZE;

  for (i=0; i<numblocks; i++) {
    if ((xor = get_xor(dbuf+i*BLOCKSIZE)) == NULL) return -1;

    /* search for xor in hash */
    HASH_FIND(hh, XORHASH, xor, sizeof(xor_t), h);
    if (h == NULL) {
      h = (xorhash_t *)malloc(sizeof(xorhash_t));
      h->xor = *xor;
      h->id  = ID++;
      h->cnt = 1;
      HASH_ADD(hh, XORHASH, xor, sizeof(xor_t), h);

      // 1 bit reserved for compression header
      if (ID > (1<<(8*sizeof(id_t)-2))+1) {
	fprintf(stderr, "ERROR: hash_xor: ID exceeds an int\n");
	return -1;
      }

      if (ID > MEMOTBL.size) {
	if ((MEMOTBL.tbl = realloc((void *)MEMOTBL.tbl,
				   (MEMOTBL.size + MEMO_NUMINST)*
				   sizeof(memo_t *))) == 0) {
	  perror("ERROR: hash_xor: realloc");
	  return -1;
	}
	memset((void *)(MEMOTBL.tbl + MEMOTBL.size), 0,
	       MEMO_NUMINST*sizeof(memo_t *));
	MEMOTBL.size += MEMO_NUMINST;
      }
    }
    else h->cnt++;

    if (SEQBUF.idx >= SEQ_BUFSIZE) {
      fprintf(stderr, "ERROR: hash_xor: SEQ_BUFSIZE too small\n");
      return -1;
    }
    SEQBUF.buf[SEQBUF.idx] = h->id;
    SEQBUF.idx++;
  }
  return numblocks * sizeof(id_t);
}

int
memo_add(id_t id, off_t offset)
{
  memo_t *d_new, *d, *d_prev;

  if ((d_new = (memo_t *)malloc(sizeof(memo_t))) == 0) {
    fprintf(stderr, "ERROR: memo_add: malloc\n");
    return -1;
  }
  d_new->offset = offset;
  d_new->next   = NULL;

  d_prev = 0;
  for (d=MEMOTBL.tbl[id]; d; d=d->next) d_prev = d;
  if (d_prev) d_prev->next = d_new;
  else        MEMOTBL.tbl[id] = d_new;

  return 0;
}

int
compress_seqbuf(byte *obuf, int endflag) {
  int           i, rc;
  unsigned int  end, len, maxlen;
  unsigned int *sbuf_cur, *sbuf_prev;
  memo_t       *d, *xor;
  unsigned int *obuf_int;

  if (endflag) end = SEQBUF.idx;
  else {
    if (SEQBUF.idx - SEQBUF.cdx < SEQ_MAXLEN) return 0;
    end = SEQBUF.idx - SEQ_MAXLEN;
  }

  obuf_int = (unsigned int *)obuf;

  rc = 0;
  while (SEQBUF.cdx < end) {
    maxlen = 1;
    sbuf_cur = SEQBUF.buf + SEQBUF.cdx;

    for (d=MEMOTBL.tbl[*sbuf_cur]; d; d=d->next) {
      len = 1;
      sbuf_prev = SEQBUF.buf + d->offset;
      while (sbuf_cur[len] == sbuf_prev[len]) {
	if (SEQBUF.cdx + len == SEQBUF.idx) {
	  if (endflag) break;
	  else         return rc*sizeof(id_t);
	}

	len++;

	if (len == SEQ_MAXLEN) {
	  fprintf(stderr, "length of pattern exceeds %d\n",
		  SEQ_MAXLEN);
	  break;
	}
      }
      if (len > maxlen) {
	maxlen = len;
	xor = d;
      }
    }

    if (maxlen == 1) {
      obuf_int[rc++] = *sbuf_cur;

      DLOG("instnum %jd\t%d\n", SEQBUF.cdx, *sbuf_cur);
    }
    else {      
      obuf_int[rc++] = maxlen | (1<<(8*sizeof(id_t)-1));
      obuf_int[rc++] = xor->offset;

      DLOG("instnum %jd\t<%jd, %d>\n",
      	   SEQBUF.cdx, xor->offset, maxlen);
    }
    for (i=0; i<maxlen; i++) {
      if (memo_add(SEQBUF.buf[SEQBUF.cdx + i],
		      SEQBUF.cdx + i) < 0) return -1;
    }
    SEQBUF.cdx += maxlen;
  }

  return rc*sizeof(id_t);
}

int
output_xorhash(byte *obuf, unsigned int bufsize)
{
  xorhash_t *h;
  int        i, idx=0, rc=0;

  DLOG("\nXOR HASH\n");

  for (h=XORHASH; h!=NULL; h=h->hh.next) {
    obuf[idx++] = h->xor.hdr;
    for (i=0; i<h->xor.rsz; i++)
      obuf[idx++] = h->xor.reg[i];
    for (i=0; i<h->xor.msz; i++)
      obuf[idx++] = h->xor.mem[i];

    DLOG("hashID %d\t0x%x | ", h->id, h->xor.hdr);
    for (i=0; i<h->xor.rsz; i++)
      DLOG("0x%x ", h->xor.reg[i]);
    DLOG("| ");
    for (i=0; i<h->xor.msz; i++)
      DLOG("0x%x ", h->xor.mem[i]);
    DLOG("\n");

    if (idx > bufsize-sizeof(xorhash_t)) {
      if (fwrite(obuf, 1, idx, stdout) != idx) {
	perror("ERROR: write");
	return -1;
      }
      rc += idx;
      idx = 0;
    }
  }
  if (idx) {
    if (fwrite(obuf, 1, idx, stdout) != idx) {
      perror("ERROR: write");
      return -1;
    }
    rc += idx;
  }
  DLOG("\n");

  STATS.alphabet_size = rc;
  return rc;
}

void
print_stats()
{
  fprintf(stderr, "GENERAL STATS\n");
  fprintf(stderr, "Number of bytes of initial state vector: %d\n",
	  BLOCKSIZE);
  fprintf(stderr, "Number of instructions: %d\n",
	  STATS.num_inst);
  fprintf(stderr, "Number of bytes of instruction sequence: %d\n",
	  STATS.sequence_size);
  fprintf(stderr, "Number of uniq XORs: %d\n",
	  STATS.num_uniq);
  fprintf(stderr, "Number of bytes of XOR hash: %d\n",
	  STATS.alphabet_size);
}

int 
main(int argc, char **argv)
{
  int          i, fd, n, rc;
  unsigned int bufsize, bufsize_read;
  byte        *ibuf, *dbuf, *obuf, *ibuf_read;
  id_t         seqsize;
  
  bufsize = BLOCKSIZE * BUF_NUMBLOCKS;
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&dbuf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&obuf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  fd = open(SEQ_FILEPATH, O_RDWR | O_CREAT | O_TRUNC,
	    (mode_t)0600);
  if (fd < 0) {
    perror("ERROR: open");
    return -1;
  }
  if (lseek(fd, SEQ_FILESIZE-1, SEEK_SET) < 0) {
    close(fd);
    perror("ERROR: lseek");
    return -1;
  }
  if (write(fd, "", 1) < 0) {
    close(fd);
    perror("ERROR: write");
    return -1;
  }
  SEQBUF.buf = (id_t *)mmap(0, SEQ_FILESIZE,
				    PROT_READ | PROT_WRITE,
				    MAP_SHARED, fd, 0);
  if (SEQBUF.buf == MAP_FAILED) {
    close(fd);
    perror("ERROR: mmap");
    return -1;
  }

  if (freopen(NULL, "rb", stdin) == 0) {
    perror("ERROR: freopen");
    return -1;
  }

  STATS.num_inst      = 0;
  STATS.sequence_size = 0;

  SEQBUF.idx = 0;
  SEQBUF.cdx = 0;

  MEMOTBL.idx  = 0;
  MEMOTBL.size = MEMO_NUMINST;
  MEMOTBL.tbl  = malloc(MEMO_NUMINST*sizeof(memo_t *));
  memset((void *)MEMOTBL.tbl, 0, MEMO_NUMINST*sizeof(memo_t *));

  ibuf_read = ibuf;
  bufsize_read = bufsize;

  DLOG("INSTRUCTION SEQUENCE\n");

  while ((n = fread(ibuf_read, 1, bufsize_read, stdin)) > 0) {
    if (n % BLOCKSIZE) {
      fprintf(stderr, "ERROR: n=%d fread not a multiple of BLOCKSIZE=%d\n", 
	      n, BLOCKSIZE);
      return -1;
    }

    /* initial sv */
    if (ibuf == ibuf_read) {
      /* save 4 bytes for size of sequence */
      if (fwrite(ibuf, 1, BLOCKSIZE + sizeof(id_t), stdout)
	  != BLOCKSIZE + sizeof(id_t)) {
	perror("ERROR: write");
	return -1;
      }
    }
    else n += BLOCKSIZE;
    
    /* xor sv */
    if ((rc = bitxor_buf(ibuf, dbuf, n, BLOCKSIZE)) != n-BLOCKSIZE) {
      fprintf(stderr, "ERROR: n=%d but only did %d bytes (BLOCKSIZE=%d)\n",
	      n, rc, BLOCKSIZE);
      return -1;
    }
    STATS.num_inst += rc/BLOCKSIZE;

    /* hash xor */
    if (hash_xor(dbuf, rc) < 0) return -1;

    /* compress and output a block of instruction sequence */
    if ((rc = compress_seqbuf(obuf, 0)) < 0) return -1;
    if (fwrite(obuf, 1, rc, stdout) != rc) {
      perror("ERROR: write");
      return -1;
    }
    STATS.sequence_size += rc;

    /* copy over the last sv for bitxor with the next block */
    for (i=0; i<BLOCKSIZE; i++){
      ibuf[i] = ibuf[n-BLOCKSIZE+i];
    }
    ibuf_read = ibuf + BLOCKSIZE;
    bufsize_read = bufsize - BLOCKSIZE;
  }
  STATS.num_uniq = ID;

  /* compress and output last block of instruction sequence */
  if (SEQBUF.cdx != SEQBUF.idx) {
    if ((rc = compress_seqbuf(obuf, 1)) < 0) return -1;
    if (fwrite(obuf, 1, rc, stdout) != rc) {
      perror("ERROR: fwrite");
      return -1;
    }
    STATS.sequence_size += rc;
  }

  /* output sequence size at the beginning of sequence */
  if (fseek(stdout, BLOCKSIZE, SEEK_SET) < 0) {
    perror("ERROR: fseek");
    return -1;
  }
  seqsize = STATS.sequence_size/sizeof(id_t);
  if (fwrite(&seqsize, sizeof(id_t), 1, stdout) != 1) {
    perror("ERROR: fwrite");
    return -1;
  }
  if (fseek(stdout, 0, SEEK_END) < 0) {
    perror("ERROR: fseek");
    return -1;
  }

  /* output xorhash */
  if (output_xorhash(obuf, bufsize) < 0) return -1;

  /* print general statistics */
  print_stats();
  
  fclose(stdin);

  if (munmap(SEQBUF.buf, SEQ_FILESIZE)) {
    perror("ERROR: munmap");
    close(fd);
    return -1;
  }
  close(fd);

  return 0;
}
