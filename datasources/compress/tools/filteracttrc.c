#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "../uthash/src/uthash.h"

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

#ifdef __6502__
#include "../trace6502.h"
#endif
#ifdef __BOCHS32__
#include "../tracebochs32.h"
#endif
#include "../traceformat.h"

typedef uint8_t  byte;
typedef uint32_t id_t;

#define SEQ_BUFSIZE  1000000000
#define SEQ_MAXLEN   2147483647  // length is an unsigned int
                                 // minus the top bit
#define SEQ_FILESIZE (SEQ_BUFSIZE * sizeof(id_t))

#define BUF_NUMACTIONS  1048704

typedef struct {
  action_t     act;
  id_t         id;
  id_t         fid;
  unsigned int cnt;
  char         mask;
  UT_hash_handle hh;
} acthash_t;

acthash_t *ACTIONHASH = NULL;
id_t ID  = 0;
id_t FID = 0;

typedef struct {
  id_t *buf;
  off_t idx;
} seqbuf_t;

seqbuf_t SEQBUF;

id_t SEQNUM;

void
add_actionhash_id(id_t id)
{
  acthash_t *h;

  h = (acthash_t *)malloc(sizeof(acthash_t));
  h->id  = id;
  h->cnt = 1;

  HASH_ADD(hh, ACTIONHASH, id, sizeof(id_t), h);
}

int
init_seqbuf(id_t *sbuf)
{
  int          i;
  acthash_t   *h;
  unsigned int length, start;
  off_t        seq_idx;

  i=0; SEQBUF.idx = 0;
  while (i < SEQNUM) {
    if (~sbuf[i]>>(8*sizeof(id_t)-1)) {
      HASH_FIND(hh, ACTIONHASH, sbuf+i, sizeof(id_t), h);
      if (h == NULL) add_actionhash_id(sbuf[i]);
      else           h->cnt++;

      SEQBUF.buf[SEQBUF.idx++] = sbuf[i++];
    }
    else { // repeated pattern: <length,start>
      length = sbuf[i++] & ~(1<<(8*sizeof(id_t)-1));
      start = sbuf[i++];
      
      for (seq_idx=start; seq_idx<start+length; seq_idx++) {
	SEQBUF.buf[SEQBUF.idx++] = SEQBUF.buf[seq_idx];

	HASH_FIND(hh, ACTIONHASH, SEQBUF.buf+seq_idx, sizeof(id_t), h);
	if (h == NULL) {
	  fprintf(stderr, "ERROR: id does not exist in hash\n");
	  return -1;
	}
	h->cnt++;
      }
    }
  }
  return 0;
}

int
build_hash(byte *ibuf, int size)
{
  byte      *ibuf_hash, *ibuf_end;
  action_t  *a;
  acthash_t *h;
  int        i;

  ibuf_hash = ibuf;
  ibuf_end  = ibuf + size;

  while (ibuf_hash < ibuf_end) {
    a = (action_t *)ibuf_hash;
    ibuf_hash += ACTIONSIZE;

    HASH_FIND(hh, ACTIONHASH, &ID, sizeof(id_t), h);
    if (!h) {
      fprintf(stderr, "ERROR: %s: action ID does not exist in sequence\n",
	      __func__);
      return -1;
    }
    h->act = *a;
    ID++;

    switch (a->hdr.id) {
    case T_INSTRUCTION:
      DLOG("INSTRUCTION:0x");
      break;
    case T_MEM_RD:
      DLOG("MEM:RD:");
      break;
    case T_MEM_WR:
      DLOG("MEM:WR:");
      break;
    case T_REG_RD:
      DLOG("REG:RD:");
      break;
    case T_REG_WR:
      DLOG("REG:WR:");
      break;
    case T_INTERRUPT:
      DLOG("INTERRUPT:0x");
      break;
    }
    if (a->hdr.id == T_INSTRUCTION ||
	a->hdr.id == T_INTERRUPT) {
      for (i=0; i<a->hdr.len; i++)
	DLOG("%02x", a->data[i]);
    }
    else {
      DLOG("0x%" PSML_ADDR_FMT ":0x", a->addr);
      for (i=0; i<a->hdr.len; i++)
	DLOG("%02x", a->val[i]);
    }
    DLOG("\n");
  }
  return (ibuf + size - ibuf_hash);
}

int
sort_acthash_cnt(acthash_t *a, acthash_t *b)
{
  return (b->cnt - a->cnt);
}

void
init_fid(id_t *sbuf)
{
  acthash_t *h;

  HASH_SORT(ACTIONHASH, sort_acthash_cnt);
  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    /* assign a lower ID value to a more frequent action */
    h->fid = FID++;
  }
}

int
print_action(FILE *file, id_t id)
{
  acthash_t *h;
  action_t   a;
  int i;

  HASH_FIND(hh, ACTIONHASH, &id, sizeof(id_t), h);
  if (!h) {
    fprintf(stderr, "ERROR: %s: action does not exist in hash\n",
	  __func__);
    return -1;
  }
  a = h->act;

  switch (a.hdr.id) {
  case T_INSTRUCTION:
    fprintf(file, "INSTRUCTION:0x");
    break;
  case T_MEM_RD:
    fprintf(file, "MEM:RD:");
    break;
  case T_MEM_WR:
    fprintf(file, "MEM:WR:");
    break;
  case T_REG_RD:
    fprintf(file, "REG:RD:");
    break;
  case T_REG_WR:
    fprintf(file, "REG:WR:");
    break;
  case T_INTERRUPT:
    fprintf(file, "INTERRUPT:0x");
    break;
  }
  if (a.hdr.id == T_INSTRUCTION ||
      a.hdr.id == T_INTERRUPT) {
    for (i=0; i<a.hdr.len; i++)
      fprintf(file, "%02x", a.data[i]);
  }
  else {
    fprintf(file, "0x%" PSML_ADDR_FMT ":0x", a.addr);
    for (i=0; i<a.hdr.len; i++)
      fprintf(file, "%02x", a.val[i]);
  }
  fprintf(file, "\n");

  return 0;
}

void
init_addr_actions(int mask, psml_address_t addr)
{
  acthash_t *h;

  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    if (h->act.addr == addr)
      h->mask |= mask;
    else
      h->mask &= ~mask;
  }
}

void
init_io_actions(int mask)
{
  init_addr_actions(mask, CONSOLE_IOADDR);
}

int
print_actions(char *filter, char mask)
{
  int        i;
  FILE      *filterfile;
  acthash_t *h;
  off_t      idx=1;

  filterfile = fopen(filter, "w+");
  fprintf(filterfile, "#idx #id #fid #r/w\n");

  for (i=0; i<SEQBUF.idx; i++) {
    HASH_FIND(hh, ACTIONHASH, SEQBUF.buf+i, sizeof(id_t), h);
    if (h == NULL) {
      fprintf(stderr, "ERROR: action ID=%d does not exist in hash\n",
	      *(SEQBUF.buf+i));
      return -1;
    }
    if ((h->mask & mask) == mask) {
      fprintf(filterfile, "%jd %u %u ", idx, h->id, h->fid);
      if (h->act.hdr.id == T_MEM_RD || h->act.hdr.id == T_REG_RD)
	fprintf(filterfile, "RD\n");
      if (h->act.hdr.id == T_MEM_WR || h->act.hdr.id == T_REG_WR)
	fprintf(filterfile, "WR\n");
    }
    idx++;
  }
  return 0;
}

static void
print_usage(char *argv0)
{
  fprintf(stderr, "USAGE: %s [compressed trace file] [filtered output file]\n"
	  "  -i      filter I/O actions\n"
	  "  -s <io> segment the trace by I/O\n",
	  argv0);
}

int
main(int argc, char **argv)
{
  int          opt, iflag=0, n, rc, mask=0;
  char        *trace=0, *filter=0;
  FILE        *tracefile;
  unsigned int seqsize, bufsize;
  byte        *ibuf, sv[SVSIZE];
  id_t        *sbuf;

  while ((opt = getopt(argc, argv, "i")) != -1) {
    switch (opt) {
    case 'i':
      iflag = 1; break;
    default:
      abort();
    }
  }
  if (optind+1 < argc) {
    trace  = argv[optind];
    filter = argv[optind+1];
  } else {
    print_usage(argv[0]);
    return -1;
  }

  if (!iflag) {
    print_usage(argv[0]);
    return -1;
  }

  bufsize = BUF_NUMACTIONS * sizeof(id_t);
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  SEQBUF.buf = (id_t *)mmap(0, SEQ_FILESIZE,
			    PROT_READ | PROT_WRITE,
			    MAP_ANONYMOUS | MAP_PRIVATE,
			    -1, 0);
  if (SEQBUF.buf == MAP_FAILED) {
    perror("ERROR: mmap");
    return -1;
  }

  if ((tracefile = fopen(trace, "r")) == 0) {
    perror("ERROR: fopen");
    return -1;
  }

  /* initial SV */
  if ((n = fread(sv, 1, SVSIZE, tracefile)) < SVSIZE) {
    fprintf(stderr, "ERROR: fread %d less than SVSIZE=%d\n",
	    n, SVSIZE);
    return -1;
  }

  /* sequence size */
  if (fread(&SEQNUM, sizeof(id_t), 1, tracefile) < 1) {
    perror("fread");
    return -1;
  }
  seqsize = SEQNUM * sizeof(id_t);

  if (posix_memalign((void *)&sbuf, 4096, seqsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  /* sequence of IDs */
  rc = SEQNUM;
  while ((n = fread(sbuf, sizeof(id_t), rc, tracefile)) > 0) rc -= n;
  if (rc) {
    fprintf(stderr, "ERROR: fread: %d less than SEQNUM=%d\n",
	    SEQNUM-rc, SEQNUM);
    return -1;
  }
  if (init_seqbuf(sbuf) < 0) return -1;

  /* hash table of actions */
  bufsize = BUF_NUMACTIONS * ACTIONSIZE;
  while ((n = fread(ibuf, 1, bufsize, tracefile)) > 0) {
    if (n % ACTIONSIZE) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ACTIONSIZE=%lu\n",
	      __func__, n, ACTIONSIZE);
      return -1;
    }
    if ((rc = build_hash(ibuf, n)) < 0) return -1;
  }
  init_fid(sbuf);

  /* filter out I/O actions */
  if (iflag) {
    mask = 1;
    init_io_actions(mask);
  }
  print_actions(filter, mask);

  fclose(tracefile);

  if (munmap(SEQBUF.buf, SEQ_FILESIZE)) {
    perror("ERROR: munmap");
  }
  
  return 0;
}
