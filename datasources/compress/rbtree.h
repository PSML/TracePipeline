/*
 * BU CAS CS520, Fall 2011
 *
 * Katherine Zhao
 *
 * Assignment 1, Exercise 3: rbtree.h
 */

#define RED 1
#define BLK 0

typedef unsigned char byte;

typedef struct {
  byte hdr;
  byte reg[7];
  byte mem[5*3+1];
  byte rsz;
  byte msz;
} diff_t;

// Datatype for red-black trees
typedef struct rbtree {
  int color;
  int key; diff_t *diff;
  struct rbtree *root;
  struct rbtree *parent;
  struct rbtree *left, *right;
  struct rbtree *huff1, *huff2;
  struct rbtree *nil;
} rbtree_t;

// methods
rbtree_t *rbtree_init(void);
rbtree_t *rbtree_new(rbtree_t *T, int k, diff_t *d,
		     rbtree_t *h1, rbtree_t *h2);
rbtree_t *rbtree_insert(rbtree_t *T, rbtree_t *z);
void rbtree_delete(rbtree_t *T, rbtree_t *z);
rbtree_t *rbtree_minimum(rbtree_t *T, rbtree_t *z);
int rbtree_check(rbtree_t *T, rbtree_t *rbt);
