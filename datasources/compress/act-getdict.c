#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthash/src/uthash.h"

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

#ifdef __6502__
#include "trace6502.h"
#endif
#ifdef __BOCHS32__
#include "tracebochs32.h"
#endif
#include "traceformat.h"

typedef uint8_t byte;
typedef uint16_t myid_t;

typedef struct {
  action_t     act;
  myid_t       id;
  myid_t       fid;
  unsigned int cnt;
  UT_hash_handle hh;
} acthash_t;
acthash_t *ACTIONHASH = NULL;

typedef struct {
  unsigned int num_act;
  unsigned int num_uniq;
} stats_t;
stats_t STATS;

myid_t ID = 0;
#define BUF_NUMACTIONS 1048704
#define HASHSIZE (sizeof(acthash_t))

byte *
read_initsv(FILE *in, byte *buf)
{
  int n;
  
  if ((n = fread(buf, 1, SVSIZE, in)) < SVSIZE) {
    fprintf(stderr, "ERROR: %s: fread: n=%d less than SVSIZE=%d\n",
	    __func__, n, SVSIZE);
    return 0;
  }
  return buf;
}

enum HashKeys {HASH_ACT, HASH_FID};
enum HashKeys HashKey;

int
hash_dictionary(FILE *dfile)
{
  int        n, i, numhash;
  uint32_t   bufsize;
  byte *buf;
  acthash_t *h;

  bufsize = ACTIONSIZE * BUF_NUMACTIONS;
  if (posix_memalign((void *)&buf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }
  while ((n = fread(buf, 1, bufsize, dfile)) > 0) {
    if (n % HASHSIZE) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of HASHSIZE=%lu\n", 
	      __func__, n, HASHSIZE);
      return -1;
    }
    numhash = n/HASHSIZE;

    for (i=0; i<numhash; i++) {
      h = (acthash_t *)buf + i;
      if (HashKey == HASH_ACT) {
	HASH_ADD(hh, ACTIONHASH, act, ACTIONSIZE, h);
      } else if (HashKey == HASH_FID) {
	HASH_ADD(hh, ACTIONHASH, fid, sizeof(myid_t), h);
      }
    }
  }
  return 0;
}

int
hash_actions(byte *buf, int bufsize)
{
  int        i, numactions;
  action_t  *action;
  acthash_t *h;

  numactions = bufsize/ACTIONSIZE;

  for (i=0; i<numactions; i++) {
    action = (action_t *)buf + i;

    /* search for action in hash */
    HASH_FIND(hh, ACTIONHASH, action, ACTIONSIZE, h);
    if (h == NULL) {
      h = (acthash_t *)malloc(sizeof(acthash_t));
      h->act = *action;
      h->id  = ++ID;
      h->cnt = 1;
      HASH_ADD(hh, ACTIONHASH, act, ACTIONSIZE, h);

      // 1 bit reserved for compression header
      if (ID > (1<<(8*sizeof(myid_t)-2))+1) {
	fprintf(stderr, "ERROR: %s: ID exceeds an int\n",
		__func__);
	return -1;
      }
    }
    else h->cnt++;
  }
  return (numactions * sizeof(myid_t));
}

int
sort_acthash_cnt(acthash_t *a, acthash_t *b)
{
  return (b->cnt - a->cnt);
}

void
init_fid()
{
  acthash_t *h;
  myid_t     fid=0;

  HASH_SORT(ACTIONHASH, sort_acthash_cnt);
  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    /* assign a lower ID value to a more frequent action */
    h->fid = ++fid;
  }
}

int
output_actionhash(byte *buf, unsigned int bufsize, FILE *dfile)
{
  acthash_t *h;
  int        idx=0, rc=0;

  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    memcpy(buf + idx, h, HASHSIZE);
    idx += HASHSIZE;

    if (idx > bufsize-HASHSIZE) {
      if (fwrite(buf, 1, idx, dfile) != idx) {
	perror("ERROR: output_actionhash: fwrite");
	return -1;
      }
      rc += idx;
      idx = 0;
    }
  }
  if (idx) {
    if (fwrite(buf, 1, idx, dfile) != idx) {
      perror("ERROR: output_actionhash: fwrite");
      return -1;
    }
    rc += idx;
  }
  return rc;
}

void
print_action(acthash_t *h)
{
  int i;

  if (h) {
    fprintf(stdout, "FID %d (%d)\tID %d\t\t",
	    h->fid, h->cnt, h->id);
    switch (h->act.hdr.id)
      {
      case T_INSTRUCTION:
	fprintf(stdout, "INSTRUCTION: opcode:0x");
	break;
      case T_MEM_RD:
	fprintf(stdout, "MEM RD: ");
	break;
      case T_MEM_WR:
	fprintf(stdout, "MEM WR: ");
	break;
      case T_REG_RD:
	fprintf(stdout, "REG RD: ");
	break;
      case T_REG_WR:
	fprintf(stdout, "REG WR: ");
	break;
      }
    if (h->act.hdr.id == T_INSTRUCTION ||
	h->act.hdr.id == T_INTERRUPT) {
      for (i=0; i<h->act.hdr.len; i++)
	fprintf(stdout, "%02x", h->act.data[i]);
      fprintf(stdout, "\n");
    }
    else {
      fprintf(stdout, "addr:0x%04x val:0x", h->act.addr);
      for (i=0; i<h->act.hdr.len; i++)
	fprintf(stdout, "%02x", h->act.val[i]);
      fprintf(stdout, "\n");
    }
  }
}

void
print_actionhash(void)
{
  acthash_t *h;

  for (h=ACTIONHASH; h!=NULL; h=h->hh.next) {
    print_action(h);
  }
}

void
print_stats()
{
  fprintf(stderr, "GENERAL STATS\n");
  fprintf(stderr, "Number of actions: %d\n",
	  STATS.num_act);
  fprintf(stderr, "Number of uniq actions: %d\n",
	  STATS.num_uniq);
}

int
print_sequence(FILE *in, byte *buf, uint32_t bufsize, int fid)
{
  int i, n, rc=0;
  action_t *action;
  acthash_t *h;

  while ((n = fread(buf, 1, bufsize, in)) > 0) {
    if (n % ACTIONSIZE) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ACTIONSIZE=%lu\n", 
	      __func__, n, ACTIONSIZE);
      return -1;
    }
    /* print sequence */
    for (i=0; i<n/ACTIONSIZE; i++) {
      action = (action_t *)buf + i;
      HASH_FIND(hh, ACTIONHASH, action, ACTIONSIZE, h);
      if (h == NULL) {
	fprintf(stderr, "ERROR: %s: action not found in hash\n", __func__);
	return -1;
      }
      if (fid) {
	if (fwrite(&h->fid, 1, sizeof(myid_t), stdout) != sizeof(myid_t)) {
	  perror("ERROR: print_sequence: fwrite");
	  return -1;
	}
      } else {
	if (fwrite(&h->id, 1, sizeof(myid_t), stdout) != sizeof(myid_t)) {
	  perror("ERROR: print_sequence: fwrite");
	  return -1;
	}
      }
    }
    rc += n;
  }
  return rc;
}

int
print_param_seq(FILE *in, byte *buf, uint32_t bufsize)
{
  int i, n, rc=0;
  myid_t *fid;
  acthash_t *h;
  
  while ((n = fread(buf, 1, bufsize, in)) > 0) {
    if (n % sizeof(myid_t)) {
      fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ID size=%lu\n", 
	      __func__, n, sizeof(myid_t));
      return -1;
    }
    /* print parameterized sequence */
    for (i=0; i<n/sizeof(myid_t); i++) {
      fid = (myid_t *)buf + i;
      HASH_FIND(hh, ACTIONHASH, fid, sizeof(myid_t), h);
      if (h == NULL) {
	fprintf(stderr, "ERROR: %s: fid not found in hash\n", __func__);
	return -1;
      }
      // NOT IMPLEMENTED
    }
    rc += n;
  }
  return rc;
}

static void
print_usage(char *argv0)
{
  fprintf(stderr, "USAGE: %s [flag] < [file]\n"
	  "  -d [ofile] takes in trace [file], outputs dictionary to [ofile], prints dictionary to stdout\n"
	  "  -s [dfile] takes in trace [file], given dictionary [dfile], outputs sequence to stdout\n"
	  "  NOT IMPLEMENTED: -p [dfile] parameterizes sequence [file] given dictionary [dfile], outputs to stdout\n",
	  argv0);
}

int 
main(int argc, char **argv)
{
  int          opt, n, dflag=0, sflag=0, pflag=0, fflag=0;
  FILE        *dfile;
  unsigned int bufsize;
  byte        *buf;

  while ((opt = getopt(argc, argv, "dspfh")) != -1) {
    switch (opt) {
    case 'd':
      dflag = 1; break;
    case 's':
      sflag = 1; break;
    case 'p':
      pflag = 1; break;
    case 'f':
      fflag = 1; break;
    case 'h':
      print_usage(argv[0]);
      return -1;
    default:
      abort();
    }
  }
  bufsize = ACTIONSIZE * BUF_NUMACTIONS;
  if (posix_memalign((void *)&buf, 4096, bufsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (dflag) {
    /* output dictionary to dfile */
    if (optind < argc) dfile = fopen(argv[optind], "w");
    else               dfile = stdout;

    /* trace is coming in from stdin */
    if (freopen(NULL, "rb", stdin) == 0) {
      perror("ERROR: freopen");
      return -1;
    }
    if (SVSIZE) {
      if (read_initsv(stdin, buf) == 0) return -1;
    }

    STATS.num_act = 0;
    while ((n = fread(buf, 1, bufsize, stdin)) > 0) {
      if (n % ACTIONSIZE) {
	fprintf(stderr, "ERROR: %s: fread: n=%d not a multiple of ACTIONSIZE=%lu\n", 
		__func__, n, ACTIONSIZE);
	return -1;
      }
      STATS.num_act += (n/ACTIONSIZE);

      /* hash actions */
      if (hash_actions(buf, n) < 0) return -1;
    }
    STATS.num_uniq = ID;

    init_fid();

    /* output actionhash */
    if (output_actionhash(buf, bufsize, dfile) < 0) return -1;

    /* print general statistics */
    print_stats();

    /* print actionhash to stdout */
    print_actionhash();
  
    fclose(stdin);
    return 0;
  }
  if (sflag || pflag) {
    /* open dictionary file */
    if (optind < argc) dfile = fopen(argv[optind], "r");
    else {
      print_usage(argv[0]);
      return -1;
    }
  }
  if (sflag) {
    HashKey = HASH_ACT;
    if (hash_dictionary(dfile) < 0) return -1;

    /* trace is coming in from stdin */
    if (freopen(NULL, "rb", stdin) == 0) {
      perror("ERROR: freopen");
      return -1;
    }
    if (SVSIZE) {
      if (read_initsv(stdin, buf) == 0) return -1;
    }
    if (fflag) {
      if (print_sequence(stdin, buf, bufsize, 1) < 0) return -1;
    } else {
      if (print_sequence(stdin, buf, bufsize, 0) < 0) return -1;
    }
    fclose(stdin);
    return 0;
  }
  if (pflag) {
    HashKey = HASH_FID;
    if (hash_dictionary(dfile) < 0) return -1;

    /* sequence is coming in from stdin */
    if (freopen(NULL, "rb", stdin) == 0) {
      perror("ERROR: freopen");
      return -1;
    }
    if (print_param_seq(stdin, buf, bufsize) < 0) return -1;
    fclose(stdin);
  }
  return 0;
}
