#ifndef __TDD_H__
#define __TDD_H__

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

typedef uint32_t id_t;
typedef int64_t trc_t;

/* String */
typedef struct {
  id_t *list;
  trc_t size;
  trc_t alloc;
} str_t;

/* Partitions */
typedef struct olist {
  trc_t offset;
  struct olist *next;
} olist_t;
typedef struct {
  olist_t **olist;
  olist_t **last;
  trc_t size;
  trc_t alloc;
} part_t;

/* Temp */
typedef struct {
  trc_t *list;
  trc_t size;
  trc_t alloc;
} temp_t;

/* Suffixes */
typedef struct {
  trc_t *cnts;
  olist_t **olist;
  olist_t **last;
  trc_t size;
  trc_t alloc;
} sort_t;

/* Stack */
typedef struct {
  trc_t tree_idx;
  olist_t *olist;
} branch_t;
typedef struct {
  branch_t *blist;
  trc_t size;
  trc_t alloc;
} stack_t;

/* Tree */
typedef union {
  uint64_t raw;
  struct __attribute__((__packed__)) {
    uint8_t  type:1;
    uint8_t  terminate:1;
    uint64_t off:38;
    uint32_t idx:24;
  };
} node_t;
enum { T_LEAF=0, T_BRANCH=1 };

typedef struct {
  node_t *nlist;
  trc_t size;
  trc_t alloc;
} tree_t;

#ifdef __DEBUG__
#  define DLOG(...) fprintf(stderr, __VA_ARGS__)
#  define DFUNC(f)  f
#else
#  define DLOG(...)
#  define DFUNC(f)
#endif

#endif
