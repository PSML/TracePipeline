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

typedef unsigned char  byte;
typedef unsigned short address;
typedef unsigned int   id_t;

#define SVSIZE        65544

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

typedef address psml_address_t;
#define PSML_VALSIZE 13

#include "traceformat.h"

#define BUF_NUMACTIONS 1048704

typedef struct {
  action_t     act;
  id_t         id;
  unsigned int cnt;
  UT_hash_handle hh;
} acthash_t;

acthash_t *ACTIONHASH = NULL;
id_t ID = 0;

typedef struct {
  unsigned int num_uniq;
  unsigned int alphabet_size;
  unsigned int num_act;
  unsigned int sequence_size;
} stats_t;

stats_t STATS;

int
hash_actions(byte *ibuf, int size)
{
  int        i, numactions;
  action_t  *action;
  acthash_t *h;

  numactions = size/ACTIONSIZE;

  for (i=0; i<numactions; i++) {
    action = (action_t *)ibuf + i;

    /* search for action in hash */
    HASH_FIND(hh, ACTIONHASH, action, ACTIONSIZE, h);
    if (h == NULL) {
      h = (acthash_t *)malloc(sizeof(acthash_t));
      h->act = *action;
      h->id  = ID++;
      h->cnt = 1;
      HASH_ADD(hh, ACTIONHASH, act, ACTIONSIZE, h);

      // 1 bit reserved for compression header
      if (ID > (1<<(8*sizeof(id_t)-2))+1) {
	fprintf(stderr, "ERROR: %s: ID exceeds an int\n",
		__func__);
	return -1;
      }

      if (ID > MEMOTBL.size) {
	if ((MEMOTBL.tbl = realloc((void *)MEMOTBL.tbl,
				   (MEMOTBL.size + MEMO_NUMINST)*
				   sizeof(memo_t *))) == 0) {
	  perror("ERROR: hash_actions: realloc");
	  return -1;
	}
	memset((void *)(MEMOTBL.tbl + MEMOTBL.size), 0,
	       MEMO_NUMINST*sizeof(memo_t *));
	MEMOTBL.size += MEMO_NUMINST;
      }
    }
    else h->cnt++;

    if (SEQBUF.idx >= SEQ_BUFSIZE) {
      fprintf(stderr, "ERROR: %s: SEQ_BUFSIZE=%d too small\n",
	      __func__, SEQ_BUFSIZE);
      return -1;
    }
    SEQBUF.buf[SEQBUF.idx] = h->id;
    SEQBUF.idx++;
  }
  return (numactions * sizeof(id_t));
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
  memo_t       *a, *action;
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

    for (a=MEMOTBL.tbl[*sbuf_cur]; a; a=a->next) {
      len = 1;
      sbuf_prev = SEQBUF.buf + a->offset;
      while (sbuf_cur[len] == sbuf_prev[len]) {
	if (SEQBUF.cdx + len == SEQBUF.idx) {
	  if (endflag) break;
	  else         return (rc * sizeof(id_t));
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
	action = a;
      }
    }

    if (maxlen == 1) {
      obuf_int[rc++] = *sbuf_cur;

      DLOG("instnum %jd\t%d\n", SEQBUF.cdx, *sbuf_cur);
    }
    else {      
      obuf_int[rc++] = maxlen | (1<<(8*sizeof(id_t)-1));
      obuf_int[rc++] = action->offset;

      DLOG("instnum %jd\t<%jd, %d>\n",
      	   SEQBUF.cdx, action->offset, maxlen);
    }
    for (i=0; i<maxlen; i++) {
      if (memo_add(SEQBUF.buf[SEQBUF.cdx + i],
		      SEQBUF.cdx + i) < 0) return -1;
    }
    SEQBUF.cdx += maxlen;
  }

  return (rc * sizeof(id_t));
}

int
output_actionhash(byte *obuf, unsigned int bufsize)
{
  acthash_t *h;
  int        i, idx=0, rc=0;

  DLOG("\nACTION HASH\n");

  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    memcpy(obuf + idx, &(h->act), ACTIONSIZE);
    idx += ACTIONSIZE;

    if (idx > bufsize-ACTIONSIZE) {
      if (fwrite(obuf, 1, idx, stdout) != idx) {
	perror("ERROR: output_actionhash: fwrite");
	return -1;
      }
      rc += idx;
      idx = 0;
    }
    DLOG("hashID %d: " h->id);
    switch (h->act.hdr.id)
      {
      case T_INSTRUCTION:
	DLOG("INSTRUCTION: opcode:0x");
	break;
      case T_MEM_RD:
	DLOG("MEM RD: ");
	break;
      case T_MEM_WR:
	DLOG("MEM WR: ");
	break;
      case T_REG_RD:
	DLOG("REG RD: ");
	break;
      case T_REG_WR:
	DLOG("REG WR: ");
	break;
      }
    if (h->act.hdr.id == T_INSTRUCTION ||
	h->act.hdr.id == T_INTERRUPT) {
      for (i=0; i<h->act.hdr.len; i++)
	DLOG("%02x", h->act.data[i]);
      DLOG("\n");
    }
    else {
      DLOG("addr:0x%04x val:0x", h->act.addr);
      for (i=0; i<h->act.hdr.len; i++)
	DLOG("%02x", h->act.val[i]);
      DLOG("\n");
    }
  }
  DLOG("\n");
  
  if (idx) {
    if (fwrite(obuf, 1, idx, stdout) != idx) {
      perror("ERROR: output_actionhash: fwrite");
      return -1;
    }
    rc += idx;
  }
  STATS.alphabet_size = rc;
  
  return rc;
}

void
print_stats()
{
  fprintf(stderr, "GENERAL STATS\n");
  fprintf(stderr, "Number of bytes of initial state vector: %d\n",
	  SVSIZE);
  fprintf(stderr, "Number of actions: %d\n",
	  STATS.num_act);
  fprintf(stderr, "Number of bytes of action sequence: %d\n",
	  STATS.sequence_size);
  fprintf(stderr, "Number of uniq actions: %d\n",
	  STATS.num_uniq);
  fprintf(stderr, "Number of bytes of action hash: %d\n",
	  STATS.alphabet_size);
}

int 
main(int argc, char **argv)
{
  int          fd, n, rc;
  unsigned int bufsize;
  byte        *ibuf, *dbuf, *obuf;
  id_t         seqnum;
  
  bufsize = ACTIONSIZE * BUF_NUMACTIONS;
  
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
	    S_IRUSR | S_IWUSR);
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

  STATS.num_act      = 0;
  STATS.sequence_size = 0;

  SEQBUF.idx = 0;
  SEQBUF.cdx = 0;

  MEMOTBL.idx  = 0;
  MEMOTBL.size = MEMO_NUMINST;
  MEMOTBL.tbl  = malloc(MEMO_NUMINST*sizeof(memo_t *));
  memset((void *)MEMOTBL.tbl, 0, MEMO_NUMINST*sizeof(memo_t *));

  /* initial sv */
  if ((n = fread(ibuf, 1, SVSIZE, stdin)) < SVSIZE) {
    fprintf(stderr, "ERROR: %s: fread: n=%d less than SVSIZE=%d\n",
	    __func__, n, SVSIZE);
  }
  /* save 4 bytes for size of sequence */
  if (fwrite(ibuf, 1, SVSIZE + sizeof(id_t), stdout)
      != SVSIZE + sizeof(id_t)) {
    perror("ERROR: fwrite");
    return -1;
  }

  DLOG("INSTRUCTION SEQUENCE\n");
  
  while ((n = fread(ibuf, 1, bufsize, stdin)) > 0) {
    if (n % ACTIONSIZE) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ACTIONSIZE=%lu\n", 
	      __func__, n, ACTIONSIZE);
      return -1;
    }
    STATS.num_act += (n/ACTIONSIZE);

    /* hash action */
    if (hash_actions(ibuf, n) < 0) return -1;

    /* compress and output a block of instruction sequence */
    if ((rc = compress_seqbuf(obuf, 0)) < 0) return -1;
    if (fwrite(obuf, 1, rc, stdout) != rc) {
      perror("ERROR: fwrite");
      return -1;
    }
    STATS.sequence_size += rc;
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
  if (fseek(stdout, SVSIZE, SEEK_SET) < 0) {
    perror("ERROR: fseek");
    return -1;
  }
  seqnum = STATS.sequence_size/sizeof(id_t);
  if (fwrite(&seqnum, sizeof(id_t), 1, stdout) != 1) {
    perror("ERROR: fwrite");
    return -1;
  }
  if (fseek(stdout, 0, SEEK_END) < 0) {
    perror("ERROR: fseek");
    return -1;
  }

  /* output actionhash */
  if (output_actionhash(obuf, bufsize) < 0) return -1;

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
