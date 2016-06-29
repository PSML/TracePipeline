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

id_t ID  = 0;
id_t FID = 0;

#ifdef __ACTION__
# define BUF_NUM_IDS 1048704

typedef struct {
  action_t act;
  id_t     id;
  id_t     fid;
  uint32_t cnt;
  UT_hash_handle hh;
} acthash_t;
acthash_t *HASH = NULL;

typedef acthash_t hash_t;

void
add_hash_id(id_t id)
{
  acthash_t *h;

  h = (acthash_t *)malloc(sizeof(acthash_t));
  h->id  = id;
  h->cnt = 1;

  HASH_ADD(hh, HASH, id, sizeof(id_t), h);
}

int
print_hash_id(FILE *file, id_t id)
{
  acthash_t *h;
  action_t   a;
  int i;

  HASH_FIND(hh, HASH, &id, sizeof(id_t), h);
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

int
build_hash(byte *ibuf, int size)
{
  byte      *ibuf_hash, *ibuf_end;
  action_t  *a;
  acthash_t *h;
  uint8_t    i;

  ibuf_hash = ibuf;
  ibuf_end  = ibuf + size;

  while (ibuf_hash < ibuf_end) {
    a = (action_t *)ibuf_hash;
    ibuf_hash += ACTIONSIZE;

    HASH_FIND(hh, HASH, &ID, sizeof(id_t), h);
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
#endif // __ACTION__

#ifdef __XOR__
# define BUF_NUM_IDS 1024

typedef struct {
  byte hdr;
  byte reg[7];
  byte mem[5*3+1];
  byte rsz;
  byte msz;
} xor_t;
typedef struct {
  xor_t    xor;
  id_t     id;
  id_t     fid;
  uint32_t cnt;
  UT_hash_handle hh;
} xorhash_t;
xorhash_t *HASH = NULL;

typedef xorhash_t hash_t;

void
add_hash_id(id_t id)
{
  xorhash_t *h;

  h = (xorhash_t *)malloc(sizeof(xorhash_t));
  h->id  = id;
  h->cnt = 1;

  HASH_ADD(hh, HASH, id, sizeof(id_t), h);
}

int
print_hash_id(FILE *file, id_t id)
{
  int        i;
  xorhash_t *h;

  HASH_FIND(hh, HASH, &id, sizeof(id_t), h);
  if (!h) {
    fprintf(stderr, "ERROR: print_xor: xor does not exist in hash\n");
    return -1;
  }
  fprintf(file, "0x%x|", h->xor.hdr);
  for (i=0; i<h->xor.rsz; i++)
    fprintf(file, "|0x%x", h->xor.reg[i]);
  fprintf(file, "|");
  for (i=0; i<h->xor.msz; i++)
    fprintf(file, "|0x%x", h->xor.mem[i]);
  fprintf(file, "\n");

  return 0;
}

xor_t *
get_xor(byte **ibuf_ptr)
{
  int    i, j;
  xor_t *xor;
  byte  *ibuf;

  ibuf = *ibuf_ptr;

  xor = (xor_t *)malloc(sizeof(xor_t));
  memset(xor, 0, sizeof(xor_t));

  i=0;
  xor->hdr = ibuf[i++];
  if (xor->hdr & 1) {      // pc
    xor->reg[xor->rsz++] = ibuf[i++];
    xor->reg[xor->rsz++] = ibuf[i++];
  }
  for (j=1; j<6; j++) {   // registers
    if (xor->hdr & (1<<j)) {
      xor->reg[xor->rsz++] = ibuf[i++];
    }
  }
  if ((xor->hdr>>6) < 3) { // memory
    for (j=0; j<(xor->hdr>>6); j++) {
      xor->mem[xor->msz++] = ibuf[i++]; // addr low
      xor->mem[xor->msz++] = ibuf[i++]; // addr high
      xor->mem[xor->msz++] = ibuf[i++]; // value
    }
  } else {
    while (ibuf[i]) {
      xor->mem[xor->msz++] = ibuf[i++];
    }
    xor->mem[xor->msz++] = ibuf[i++];
  }

  *ibuf_ptr = ibuf + i;
  
  return xor;
}

int
build_hash(byte *ibuf, int size, int endflag)
{
  byte      *ibuf_hash, *ibuf_end;
  xor_t     *xor;
  xorhash_t *h;
  int        i;

  if (endflag) ibuf_end = ibuf + size;
  else         ibuf_end = ibuf + size - sizeof(xor_t);

  ibuf_hash = ibuf;

  while (ibuf_hash < ibuf_end) {
    xor = get_xor(&ibuf_hash);

    HASH_FIND(hh, HASH, &ID, sizeof(id_t), h);
    if (!h) {
      fprintf(stderr, "ERROR: %s: xor ID does not exist in sequence\n",
	      __func__);
      return -1;
    }
    h->xor = *xor;
    ID++;

    DLOG("0x%x|", h->xor.hdr);
    for (i=0; i<h->xor.rsz; i++)
      DLOG("|0x%x", h->xor.reg[i]);
    DLOG("|");
    for (i=0; i<h->xor.msz; i++)
      DLOG("|0x%x", h->xor.mem[i]);
    DLOG("\n");
  }
  return (ibuf + size - ibuf_hash);
}
#endif // __XOR__

typedef struct {
  id_t *buf;
  off_t idx;
} seqbuf_t;

seqbuf_t SEQBUF;

typedef struct {
  off_t    start;
  uint32_t len;
} pat_t;

typedef struct {
  pat_t pat;
  off_t idx;
  UT_hash_handle hh;
} pathash_t;

pathash_t *PATHASH;

id_t SEQNUM;

int
init_seqbuf(id_t *sbuf)
{
  id_t     i;
  hash_t  *h;
  uint32_t length;
  off_t    start, seq_idx;

  i=0; SEQBUF.idx = 0;
  while (i < SEQNUM) {
    if (~sbuf[i]>>(8*sizeof(id_t)-1)) {
      HASH_FIND(hh, HASH, sbuf+i, sizeof(id_t), h);
      if (h == NULL) add_hash_id(sbuf[i]);
      else           h->cnt++;

      SEQBUF.buf[SEQBUF.idx++] = sbuf[i++];
    }
    else { // repeated pattern: <length,start>
      length = sbuf[i++] & ~(1<<(8*sizeof(id_t)-1));
      start = sbuf[i++];
      
      for (seq_idx=start; seq_idx<start+length; seq_idx++) {
	SEQBUF.buf[SEQBUF.idx++] = SEQBUF.buf[seq_idx];

	HASH_FIND(hh, HASH, SEQBUF.buf+seq_idx, sizeof(id_t), h);
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

void
add_pathash_idx(off_t start, uint32_t len, off_t idx)
{
  pat_t      p;
  pathash_t *h;

  p.start = start;
  p.len   = len;
  
  h = (pathash_t *)malloc(sizeof(pathash_t));
  h->pat = p;
  h->idx = idx;

  HASH_ADD(hh, PATHASH, idx, sizeof(unsigned int), h);
}

int
sort_acthash_cnt(hash_t *a, hash_t *b)
{
  return (b->cnt - a->cnt);
}

int
sort_pathash_start(pathash_t *a, pathash_t *b)
{
  return (a->pat.start - b->pat.start);
}

int
sort_pathash_len(pathash_t *a, pathash_t *b)
{
  return (b->pat.len - a->pat.len);
}

void
init_fid(id_t *sbuf)
{
  hash_t *h;

  HASH_SORT(HASH, sort_acthash_cnt);
  for (h=HASH; h!=NULL; h=h->hh.next) {
    /* assign a lower ID value to a more frequent action */
    h->fid = FID++;
  }
}

int
print_hist(char *hist)
{
  FILE   *histfile;
  hash_t *h;

  histfile = fopen(hist, "w+");
  fprintf(histfile, "#id #count #action\n");

  for (h=HASH; h!=NULL; h=h->hh.next) {
    fprintf(histfile, "%u %u ", h->id, h->cnt);
    if (print_hash_id(histfile, h->id) < 0) {
      fclose(histfile);
      return -1;
    }
  }
  
  fclose(histfile);
  
  return 0;
}

int
print_seq(char *seq)
{
  FILE   *seqfile;
  hash_t *h;
  off_t   i, idx=1;

  seqfile = fopen(seq, "w+");
  fprintf(seqfile, "idx id fid cnt\n");

  for (i=0; i<SEQBUF.idx; i++) {
    HASH_FIND(hh, HASH, SEQBUF.buf+i, sizeof(id_t), h);
    if (h == NULL) {
      fprintf(stderr, "ERROR: id does not exist in hash\n");
      fclose(seqfile);
      return -1;
    }
    fprintf(seqfile, "%jd %u %u %u\n", idx++, h->id, h->fid, h->cnt);
    //fprintf(seqfile, "%u\n", h->id);
  }
  
  fclose(seqfile);
  
  return 0;
}

void
print_pat(id_t *sbuf, char *pat)
{
  FILE      *patfile;
  pathash_t *h;
  off_t      i, start, idx;
  int32_t    patnum=-1, patidx=-1;
  uint32_t   length;
  pat_t      curpat;

  patfile = fopen(pat, "w+");
  fprintf(patfile, "#pat #idx #len #patidx\n");

  i=0; idx=0;
  while (i < SEQNUM) {
    if (sbuf[i] >> (8*sizeof(id_t)-1)) {
      length = sbuf[i++] & ~(1 << (8*sizeof(id_t)-1));
      start = sbuf[i++];
      
      add_pathash_idx(start, length, idx);
      idx += length;
    }
    else { i++; idx++; }
  }
  HASH_SORT(PATHASH, sort_pathash_start);
  HASH_SORT(PATHASH, sort_pathash_len);
  
  curpat.start=-1; curpat.len=-1;
  for (h=PATHASH; h!=NULL; h=h->hh.next) {
    if (h->pat.len != curpat.len) {
      curpat.start = h->pat.start;
      curpat.len   = h->pat.len;
      patnum = 1;
      patidx++;
      fprintf(patfile, "%d %jd %u %d\n",
	      patnum, h->pat.start, h->pat.len, patidx);
    }
    else if (h->pat.start != curpat.start) {
      curpat.start = h->pat.start;
      patnum++;
      patidx++;
      fprintf(patfile, "%d %jd %u %d\n",
	      patnum, h->pat.start, h->pat.len, patidx);
    }
    fprintf(patfile, "%d %jd %u %d\n", patnum, h->idx, h->pat.len, patidx);
  }
  
  fclose(patfile);
}

int
print_avg(id_t *sbuf, char *avg)
{
  FILE    *avgfile;
  hash_t  *h;
  off_t    i=0, idx=1;
  id_t     id=0;
  uint32_t numbytes=0, length, l;

  avgfile = fopen(avg, "w+");
  fprintf(avgfile, "#idx #numbytes\n");

  while (i < SEQNUM) {
    if (sbuf[i] >> (8*sizeof(id_t)-1)) {
      length = sbuf[i++] & ~(1 << (8*sizeof(id_t)-1));
      i++; // next byte is the start location
      numbytes += (2 * sizeof(id_t));

      for (l=0; l<length; l++) {
	fprintf(avgfile, "%jd %f\n", idx, numbytes/(float)idx);
	idx++;
      }
    }
    else {
      HASH_FIND(hh, HASH, sbuf+i, sizeof(id_t), h);
      if (h == NULL) {
	fprintf(stderr, "ERROR: id does not exist in hash\n");
	return -1;
      }
      if (h->id == id) {
	numbytes += ACTIONSIZE;
	id++;
      }
      numbytes += sizeof(id_t);
      fprintf(avgfile, "%jd %f\n", idx, numbytes/(float)idx);
      idx++; i++;
    }
  }
  
  fclose(avgfile);
  
  return 0;
}

static void
print_usage(char *argv0)
{
  fprintf(stderr, "USAGE: %s [compressed trace file]\n"
	  "  -h [file]  output to file: histogram of trace\n"
	  "  -s [file]  output to file: sequence of trace\n"
	  "  -p [file]  output to file: patterns of trace\n"
	  "  -a [file]  output to file: average number of bytes per action\n",
	  argv0);
}

int
main(int argc, char **argv)
{
  int      opt, n, rc;
  char    *trace=0, *hist=0, *seq=0, *pat=0, *avg=0;
  FILE    *tracefile;
  uint32_t seqsize, bufsize;
  byte    *ibuf, sv[SVSIZE];
  id_t    *sbuf;

#ifdef __XOR__
  int      i;
  uint32_t ibufsize;
#endif

  while ((opt = getopt(argc, argv, "h:s:p:a:")) != -1) {
    switch (opt) {
    case 'h':
      hist = optarg; break;
    case 's':
      seq = optarg; break;
    case 'p':
      pat = optarg; break;
    case 'a':
      avg = optarg; break;
    default:
      abort();
    }
  }
  if (optind < argc) trace = argv[optind];

  if (!(trace && (hist || seq || pat || avg))) {
    print_usage(argv[0]);
    return -1;
  }

  bufsize = BUF_NUM_IDS * sizeof(id_t);
  
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
  fprintf(stderr, "trace = %s\n", trace);
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

#ifdef __ACTION__
  /* table of actions */
  bufsize = BUF_NUM_IDS * ACTIONSIZE;
  while ((n = fread(ibuf, 1, bufsize, tracefile)) > 0) {
    if (n % ACTIONSIZE) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ACTIONSIZE=%lu\n",
	      __func__, n, ACTIONSIZE);
      return -1;
    }
    if ((rc = build_hash(ibuf, n)) < 0) return -1;
  }
#endif

#ifdef __XOR__
  /* table of XORs */
  bufsize = BUF_NUM_IDS * sizeof(id_t);
  rc=0;
  while ((n = fread(ibuf+rc, 1, bufsize-rc, tracefile)) > 0) {
    ibufsize = n+rc;
    if ((rc = build_hash(ibuf, ibufsize, 0)) < 0) return -1;

    /* copy over remaining bytes */
    for (i=0; i<rc; i++) ibuf[i] = ibuf[ibufsize-rc+i];
  }
  if (build_hash(ibuf, rc, 1) != 0) return -1;
#endif
  
  init_fid(sbuf);

  if (hist) {
    if (print_hist(hist) < 0) return -1;
  }
  if (seq) {
    if (print_seq(seq) < 0) return -1;
  }
  if (pat) print_pat(sbuf, pat);
  if (avg) {
    if (print_avg(sbuf, avg) < 0) return -1;
  }

  fclose(tracefile);

  if (munmap(SEQBUF.buf, SEQ_FILESIZE)) {
    perror("ERROR: munmap");
  }
  
  return 0;
}
