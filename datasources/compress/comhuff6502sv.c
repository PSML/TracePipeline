#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "tools/bitxor.h"
#include "rbtree.h"
#include "uthash/src/uthash.h"

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

#define BLOCKSIZE     65544
#define BUF_NUMBLOCKS 1024
#define SEQ_NUMBLOCKS 4096

typedef struct {
  diff_t       key;
  unsigned int count;
  byte         nbits;
  unsigned int hcode;
  UT_hash_handle hh;
} hashdiff_t;

hashdiff_t *ALPHABET = NULL;

typedef struct {
  diff_t **diffs;
  unsigned int idx;
  unsigned int size;
} diffseq_t;

diffseq_t DIFFSEQ;

typedef struct {
  unsigned int num_inst;
  unsigned int sequence_size;
  unsigned int num_uniq;
  unsigned int alphabet_size;
} stats_t;

stats_t STATS;

diff_t *
get_diff(byte *dbuf)
{
  diff_t        *d;
  unsigned int   i;
  byte           c;
  unsigned short addr;

  d = (diff_t *)malloc(sizeof(diff_t));
  memset(d, 0, sizeof(diff_t));

  for (i=0; i<BLOCKSIZE; i++) {
    /* addr = i, val = c */
    if ((c = dbuf[i])) {
      if (i == 0) {        // pc
	d->hdr |= (1<<0);
	d->reg[d->rsz++] = c;
	i++;
	d->reg[d->rsz++] = dbuf[i];
      } else if (i == 1) { // pc
	d->hdr |= (1<<0);
	d->reg[d->rsz++] = dbuf[i-1];
	d->reg[d->rsz++] = c;
      } else if (i < 7) {  // registers
	d->hdr |= (1<<(i-1));
	d->reg[d->rsz++]= c;
      } else if (i > 7) {  // memory
	if (d->msz > 4*3) {
	  fprintf(stderr, "ERROR: more than 5 bytes of memory changed\n");
	  return NULL;
	}
	if ((d->hdr>>6) < 3) d->hdr = (((d->hdr>>6)+1)<<6) | (d->hdr & 0x3f);
	addr = i-8;
	d->mem[d->msz++] = (byte)(0xFF & addr);        // addr low
	d->mem[d->msz++] = (byte)((0xFF00 & addr)>>8); // addr high
	d->mem[d->msz++] = c;                          // value
      }
    }
  }
  /* more than 2 bytes of memory changed */
  if (d->msz > 2*3) {
    d->mem[d->msz] = 0;
    d->msz++;
  }
  return d;
}

int
hash_diff(byte *dbuf, unsigned int dbuf_size)
{
  int         i, j, numblocks, numuniq=0;
  diff_t     *k;
  hashdiff_t *h;

  numblocks = dbuf_size/BLOCKSIZE;

  if (numblocks > DIFFSEQ.size - DIFFSEQ.idx) {
    if ((DIFFSEQ.diffs = realloc((void *)DIFFSEQ.diffs,
	(DIFFSEQ.size + SEQ_NUMBLOCKS)*sizeof(diff_t *))) == 0) {
      perror("ERROR: hash_diff: realloc");
      return -1;
    }
    DIFFSEQ.size += SEQ_NUMBLOCKS;
  }

  for (i=0; i<numblocks; i++) {
    if ((k = get_diff(dbuf+i*BLOCKSIZE)) == NULL) return -1;

    /* search for key in hash */
    HASH_FIND(hh, ALPHABET, k, sizeof(diff_t), h);
    if (h == NULL) {
      h = (hashdiff_t *)malloc(sizeof(hashdiff_t));
      h->key   = *k;
      h->count = 1;
      HASH_ADD(hh, ALPHABET, key, sizeof(diff_t), h);
      numuniq++;

      DLOG("0x%x | ", k->hdr);
      for (j=0; j<k->rsz; j++)
	DLOG("0x%x ", k->reg[j]);
      DLOG("| ");
      for (j=0; j<k->msz; j++)
	DLOG("0x%x ", k->mem[j]);
      DLOG("\n");
    }
    else h->count++;

    DIFFSEQ.diffs[DIFFSEQ.idx++] = &h->key;
  }
  return numuniq;
}

int
key_count_sort(hashdiff_t *a, hashdiff_t *b)
{
  return (b->count - a->count);
}

void
rbtree_print(rbtree_t *T, rbtree_t *x) {
  if (x != T->nil) {
    DLOG("rbtree: %p [ left : %p, key: %d, diff: %p ]\n", x, x->left, x->left->key, x->left->diff);
    DLOG("rbtree: %p [ right: %p, key: %d, diff: %p ]\n", x, x->right, x->right->key, x->right->diff);
    rbtree_print(T, x->left);
    rbtree_print(T, x->right);
  }
}

void
hufftree_print(rbtree_t *x) {
  if (x->huff1 || x->huff2) {
    DLOG("hufftree: %p [ huff1: %p, key: %d, diff: %p ]\n", x, x->huff1, x->huff1->key, x->huff1->diff);
    DLOG("hufftree: %p [ huff2: %p, key: %d, diff: %p ]\n", x, x->huff2, x->huff2->key, x->huff2->diff);
    hufftree_print(x->huff1);
    hufftree_print(x->huff2);
  }
}

void
huffcode_print(byte nbits, int hcode) {
  unsigned int i;

  for (i=nbits-1; i>=0; i--) {
    if (hcode & (1<<i)) fprintf(stderr, "1");
    else                fprintf(stderr, "0");
  }
}

rbtree_t *
build_hufftree(void)
{
  hashdiff_t *h;
  rbtree_t   *T, *Tmin1, *Tmin2;//, *tmp;

  T = rbtree_init();
  Tmin1 = 0;

  /* sort alphabet in decreasing key frequency */
  HASH_SORT(ALPHABET, key_count_sort);

  DLOG("\nDIFF COUNT\n");
  for (h=ALPHABET; h!=NULL; h=h->hh.next) {
    rbtree_insert(T, rbtree_new(T, h->count, &h->key, NULL, NULL));
    DLOG("%d ", h->count);
  }
  DLOG("\n");
  DLOG("\nHASHDIFF RBTREE\n");
  DLOG("T->nil: %p, T->root: %p, root->key: %d, root->diff: %p\n", T->nil, T->root, T->root->key, T->root->diff);
  DFUNC(rbtree_print(T, T->root));

  while (T->root != T->nil) {
    Tmin1 = rbtree_minimum(T, T->root);
    rbtree_delete(T, Tmin1);
    if (T->root == T->nil) break;
    Tmin2 = rbtree_minimum(T, T->root);
    rbtree_delete(T, Tmin2);
    rbtree_insert(T, rbtree_new(T, Tmin1->key + Tmin2->key, NULL,
				Tmin1, Tmin2));
  }

  DLOG("\nHUFFMAN CODE TREE\n");
  if (Tmin1) {
    DLOG("root: %p, root->key: %d, root->diff: %p\n", Tmin1, Tmin1->key, Tmin1->diff);
    DFUNC(hufftree_print(Tmin1));
  }

  return Tmin1;
}

int
huffman_coding(rbtree_t *x, unsigned int hcode, byte nbits) {
  hashdiff_t *h;

  if (x->huff1 || x->huff2) {
    if (huffman_coding(x->huff1, (hcode<<1),   nbits+1) < 0) return -1;
    if (huffman_coding(x->huff2, (hcode<<1)+1, nbits+1) < 0) return -1;
  }
  else {
    HASH_FIND(hh, ALPHABET, x->diff, sizeof(diff_t), h);
    if (h == NULL) {
      fprintf(stderr, "ERROR: huffman_coding: diff %p not found in hash\n", x->diff);
      return -1;
    }
    h->nbits = nbits;
    h->hcode = hcode;

    DLOG("diff: %p, hcode: ", x->diff);
    DFUNC(huffcode_print(nbits, hcode));
    DLOG("\n");
  }
  return 0;
}

int
output_alphabet(byte *obuf, unsigned int bufsize)
{
  hashdiff_t *h;
  int         i, nbytes, idx=0, rc=0;

  memset((void *)obuf, 0, bufsize);

  /* number of uniq diffs */
  memcpy(&obuf[idx], &STATS.num_uniq, sizeof(unsigned int));
  idx += sizeof(unsigned int);

  for (h=ALPHABET; h!=NULL; h=h->hh.next) {
    /* huffman code */
    obuf[idx++] = h->nbits;
    if (h->nbits % 8 == 0) nbytes =  h->nbits / 8;
    else                   nbytes = (h->nbits / 8) + 1;
    memcpy(&obuf[idx], &h->hcode, nbytes);
    idx += nbytes;

    /* hashdiff */
    obuf[idx++] = h->key.hdr;
    for (i=0; i<h->key.rsz; i++)
      obuf[idx++] = h->key.reg[i];
    for (i=0; i<h->key.msz; i++)
      obuf[idx++] = h->key.mem[i];

    if (idx > bufsize-2*sizeof(hashdiff_t)) {
      if (fwrite(obuf, 1, idx, stdout) != idx) {
	perror("ERROR: output_alphabet: write");
	return -1;
      }
      rc += idx;
      idx = 0;
      memset((void *)obuf, 0, bufsize);
    }
  }
  if (idx) {
    if (fwrite(obuf, 1, idx, stdout) != idx) {
      perror("ERROR: output_alphabet: write");
      return -1;
    }
    rc += idx;
  }
  STATS.alphabet_size = rc;
  return rc;
}

int
output_sequence(byte *obuf, unsigned int bufsize)
{
  int         i, j, idx=0, bits=0, rc=0;
  hashdiff_t *h;

  memset((void *)obuf, 0, bufsize);

  for (i=0; i<DIFFSEQ.idx; i++) {
    HASH_FIND(hh, ALPHABET, DIFFSEQ.diffs[i], sizeof(diff_t), h);
    if (h == NULL) {
      fprintf(stderr, "ERROR: output_sequence: diff %p not found in hash\n", DIFFSEQ.diffs[DIFFSEQ.idx]);
      return -1;
    }
    for (j=h->nbits-1; j>=0; j--) {
      if (h->hcode & (1<<j))
	obuf[idx] |= (1<<bits);
      bits++;
      if (bits % 8 == 0) {
	idx++;
	bits = 0;
	if (idx > bufsize-2*sizeof(unsigned int)) {
	  if (fwrite(obuf, 1, idx, stdout) != idx) {
	    perror("ERROR: output_sequence: write");
	    return -1;
	  }
	  rc += idx;
	  idx = 0;
	  memset((void *)obuf, 0, bufsize);
	}
      }
    }
  }
  if (idx) {
    if (fwrite(obuf, 1, idx, stdout) != idx) {
      perror("ERROR: output_sequence: write");
      return -1;
    }
    rc += idx;
  }
  STATS.sequence_size = rc;
  return rc;
}

int 
main(int argc, char **argv)
{
  int         i, n, rc, bufsize, bufsize_read;
  byte       *ibuf, *ibuf_read, *dbuf, *obuf;
  rbtree_t   *hufftree;
  hashdiff_t *h;
  
  bufsize = BLOCKSIZE * BUF_NUMBLOCKS;
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if (posix_memalign((void *)&dbuf, 4096, bufsize) != 0) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  if ((DIFFSEQ.diffs = malloc(SEQ_NUMBLOCKS * sizeof(diff_t *))) == 0) {
    perror("ERROR: malloc");
    return -1;
  }
  DIFFSEQ.idx = 0;
  DIFFSEQ.size = SEQ_NUMBLOCKS;

  if ((obuf = (byte *)malloc(bufsize)) == 0) {
    perror("ERROR: malloc");
    return -1;
  }
  
  if (freopen(NULL, "rb", stdin) == 0) {
    perror("ERROR: freopen");
    return -1;
  }

  DLOG("\nXOR HASH\n");

  STATS.num_inst = 0;
  STATS.num_uniq = 0;
  ibuf_read = ibuf;
  bufsize_read = bufsize;

  while ((n = fread(ibuf_read, 1, bufsize_read, stdin)) > 0) {
    if (n % BLOCKSIZE) {
      fprintf(stderr, "ERROR: n=%d fread not a multiple of BLOCKSIZE=%d\n", 
	      n, BLOCKSIZE);
      return -1;
    }

    /* output initial sv */
    if (ibuf == ibuf_read) {
       if (fwrite(ibuf, 1, BLOCKSIZE, stdout) != BLOCKSIZE) {
	perror("ERROR: write");
	return -1;
      }
     }
    else n += BLOCKSIZE;

    if ((rc = bitxor_buf(ibuf, dbuf, n, BLOCKSIZE)) != n-BLOCKSIZE) {
      fprintf(stderr, "ERROR: n=%d but only did %d bytes (BLOCKSIZE=%d)\n",
	      n, rc, BLOCKSIZE);
      return -1;
    }
    STATS.num_inst += rc/BLOCKSIZE;

    if ((rc = hash_diff(dbuf, rc)) < 0) return -1;
    STATS.num_uniq += rc;

    /* copy over the last sv for bitxor with the next block */
    for (i=0; i<BLOCKSIZE; i++)
      ibuf[i] = ibuf[n-BLOCKSIZE+i];
    ibuf_read = ibuf + BLOCKSIZE;
    bufsize_read = bufsize - BLOCKSIZE;
  }

  if ((hufftree = build_hufftree()) == 0) {
    fprintf(stderr, "ERROR: build_hufftree() returns NULL ptr\n");
    return -1;
  }
  DLOG("\nHUFFMAN CODING\n");
  if (huffman_coding(hufftree, 0, 0) < 0) return -1;

  DLOG("\nFREQ\tHCODE\n");
  for (h=ALPHABET; h!=NULL; h=h->hh.next) {
    DLOG("%d\t", h->count);
    DFUNC(huffcode_print(h->nbits, h->hcode));
    DLOG("\n");
  }

  if (output_alphabet(obuf, bufsize) < 0) return -1;
  if (output_sequence(obuf, bufsize) < 0) return -1;

  fprintf(stderr, "\nGENERAL STATS\n");
  fprintf(stderr, "Number of bytes of initial state vector: %d\n",
	  BLOCKSIZE);
  fprintf(stderr, "Number of uniq XORs: %d\n",
	  STATS.num_uniq);
  fprintf(stderr, "Number of bytes of XOR hash: %d\n",
	  STATS.alphabet_size);
  fprintf(stderr, "Number of instructions: %d\n",
	  STATS.num_inst);
  fprintf(stderr, "Number of bytes of instruction sequence: %d\n\n",
	  STATS.sequence_size);

  fclose(stdin);

  return 0;
}
