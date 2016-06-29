#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
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

#define SEQ_BUFSIZE   10000000
#define SEQ_MAXLEN    2147483647  // length is an unsigned int
                                  // minus the top bit
#define SEQ_FILEPATH  "/tmp/mmapped.bin"
#define SEQ_FILESIZE  (SEQ_BUFSIZE * sizeof(id_t))

typedef struct {
  id_t *buf;
  off_t idx;
} seqbuf_t;

seqbuf_t SEQBUF;

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

int
update_sv(byte *sv, id_t id)
{
  int        i, addr, idx;
  xorhash_t *h;

  HASH_FIND(hh, XORHASH, &id, sizeof(id_t), h);
  if (h == NULL) {
    fprintf(stderr, "ERROR: update_sv: id=%d not found in hash\n", id);
    return -1;
  }

  idx=0;
  if (h->xor.hdr & 1) { // pc
    sv[0] ^= h->xor.reg[idx++];
    sv[1] ^= h->xor.reg[idx++];
  }
  for (i=1; i<6; i++) { // registers
    if (h->xor.hdr & (1<<i)) {
      sv[i+1] ^= h->xor.reg[idx++];
    }
  }
  idx=0;
  for (i=0; i<(h->xor.msz/3); i++) { // memory: each 3 bytes (addr, val)
    addr = h->xor.mem[idx] | ((h->xor.mem[idx+1]<<8) & 0xFF00);
    idx += 2;
    sv[addr+8] ^= h->xor.mem[idx++];
  }
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

  if (endflag) ibuf_end = ibuf + size;
  else         ibuf_end = ibuf + size - sizeof(xor_t);

  ibuf_hash = ibuf;

  while (ibuf_hash < ibuf_end) {
    xor = get_xor(&ibuf_hash);

    HASH_FIND(hh, XORHASH, xor, sizeof(xor_t), h);
    if (h) {
      fprintf(stderr, "ERROR: build_hash: xor exists in hash\n");
      return -1;
    }
    h = (xorhash_t *)malloc(sizeof(xorhash_t));
    h->xor = *xor;
    h->id  = ID++;
    h->cnt = 0;
    HASH_ADD(hh, XORHASH, id, sizeof(id_t), h);
  }
  return (ibuf + size - ibuf_hash);
}

int
main(int argc, char **argv)
{
  int            i, fd, n, rc;
  unsigned int   seq_numblocks, seqsize, bufsize, ibufsize;
  byte          *ibuf, sv[BLOCKSIZE];
  id_t          *sbuf;
  unsigned int   length, start;
  xorhash_t     *h;
  off_t          seq_idx;

  bufsize = BUF_NUMBLOCKS * sizeof(id_t);
  
  if (posix_memalign((void *)&ibuf, 4096, bufsize)) {
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

  /* initial SV */
  if ((n = fread(sv, 1, BLOCKSIZE, stdin)) < BLOCKSIZE) {
    fprintf(stderr, "ERROR: fread %d less than blocksize=%d\n",
	    n, BLOCKSIZE);
    return -1;
  }
  if (fwrite(sv, 1, BLOCKSIZE, stdout) != BLOCKSIZE) {
    perror("write");
    return -1;
  }

  /* sequence size */
  if (fread(&seq_numblocks, sizeof(id_t), 1, stdin) < 1) {
    perror("fread");
    return -1;
  }
  seqsize = seq_numblocks * sizeof(id_t);

  if (posix_memalign((void *)&sbuf, 4096, seqsize)) {
    perror("ERROR: posix_malloc");
    return -1;
  }

  /* sequence of IDs */
  rc = seq_numblocks;
  while ((n = fread(sbuf, sizeof(id_t), rc, stdin)) > 0) rc -= n;
  if (rc) {
    fprintf(stderr, "ERROR: fread %d less than seq_numblocks=%d\n",
	    seq_numblocks-rc, seq_numblocks);
    return -1;
  }

  /* table of XORs */
  bufsize = BUF_NUMBLOCKS * sizeof(id_t);
  rc=0;
  while ((n = fread(ibuf+rc, 1, bufsize-rc, stdin)) > 0) {
    ibufsize = n+rc;
    if ((rc = build_hash(ibuf, ibufsize, 0)) < 0) return -1;

    /* copy over remaining bytes */
    for (i=0; i<rc; i++) ibuf[i] = ibuf[ibufsize-rc+i];
  }
  if (build_hash(ibuf, rc, 1) != 0) return -1;

  DLOG("XOR HASH\n");
  for (h=XORHASH; h!=NULL; h=h->hh.next) {
    DLOG("hashID %d\t0x%x | ", h->id, h->xor.hdr);
    for (i=0; i<h->xor.rsz; i++)
      DLOG("0x%x ", h->xor.reg[i]);
    DLOG("| ");
    for (i=0; i<h->xor.msz; i++)
      DLOG("0x%x ", h->xor.mem[i]);
    DLOG("\n");
  }

  DLOG("\nINSTRUCTION SEQUENCE\n");

  /* output raw trace */
  i=0; SEQBUF.idx = 0;
  while (i < seq_numblocks) {
    DLOG("instnum %d\t", i);
    if (~sbuf[i]>>(8*sizeof(id_t)-1)) {
      DLOG("%d\n", sbuf[i]);

      SEQBUF.buf[SEQBUF.idx++] = sbuf[i];
      if (update_sv(sv, sbuf[i++]) < 0) return -1;

      if (fwrite(sv, 1, BLOCKSIZE, stdout) != BLOCKSIZE) {
	perror("ERROR: write");
	return -1;
      }
    }
    else { // repeated pattern: <length,start>
      length = sbuf[i++] & ~(1<<(8*sizeof(id_t)-1));
      start = sbuf[i++];
      DLOG("<%d, %d>\n", start, length);
      
      for (seq_idx=start; seq_idx<start+length; seq_idx++) {
	SEQBUF.buf[SEQBUF.idx++] = SEQBUF.buf[seq_idx];
	if (update_sv(sv, SEQBUF.buf[seq_idx]) < 0) return -1;

	if (fwrite(sv, 1, BLOCKSIZE, stdout) != BLOCKSIZE) {
	  perror("ERROR: write");
	  return -1;
	}
      }
    }
  }
  fclose(stdin);

  if (munmap(SEQBUF.buf, SEQ_FILESIZE)) {
    perror("ERROR: munmap");
  }
  close(fd);

  return 0;
}
