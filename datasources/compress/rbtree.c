/*
 * BU CAS CS520, Fall 2011
 *
 * Katherine Zhao
 *
 * Assignment 1, Exercise 3: rbtree.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

rbtree_t *talloc(void) {
  return (rbtree_t *) malloc(sizeof(rbtree_t));
}

/**
 * Set sentinel
 * Return a rbtree_t pointer
 */
rbtree_t *rbtree_set_sen(void) {
  rbtree_t *sen = talloc();
  sen->color = BLK;  // RED: 1, BLK: 0

  return sen;
}

/**
 * Initialize T
 * Return a rbtree_t pointer
 */
rbtree_t *rbtree_init(void) {
  rbtree_t *T = talloc();
  T->nil = rbtree_set_sen();
  T->root = T->nil;

  return T;
}

/**
 * Initialize a new rbtree
 * Return a rbtree_t pointer
 */
rbtree_t *rbtree_new(rbtree_t *T, int k, diff_t *d,
		     rbtree_t *h1, rbtree_t *h2) {
  rbtree_t *x = talloc();
  x->color = RED;  // RED: 1, BLK: 0
  x->key   = k;
  x->diff  = d;
  x->left  = T->nil;
  x->right = T->nil;
  x->huff1 = h1;
  x->huff2 = h2;

  return x;
}

/**
 * Left rotation
 *
 *    x             y
 *   / \           / \
 *  1   y         x   3
 *     / \       / \
 *    2   3     1   2
 *
 * Note: no modification needed for 1 and 3
 */
void rbtree_left_rotate(rbtree_t *T, rbtree_t *x) {
  rbtree_t *y = x->right;

  x->right = y->left;  // x->right = 2
  if (y->left != T->nil) y->left->parent = x;  // if 2 != nil, 2->parent = x

  y->parent = x->parent;
  if (x->parent == T->nil) T->root = y;  // if x = root
  else if (x == x->parent->left) x->parent->left = y;  // if x = a left child
  else x->parent->right = y;  // if x = a right child

  y->left = x;
  x->parent = y;
}

/**
 * Right rotation
 *
 *      x         y
 *     / \       / \
 *    y   3     1   x
 *   / \           / \
 *  1   2         2   3
 *
 * Note: no modification needed for 1 and 3
 */
void rbtree_right_rotate(rbtree_t *T, rbtree_t *x) {
  rbtree_t *y = x->left;

  x->left = y->right;  // x->left = 2
  if (y->right != T->nil) y->right->parent = x;  // if 2 != nil, 2->parent = x

  y->parent = x->parent;
  if (x->parent == T->nil) T->root = y;  // if x = root
  else if (x == x->parent->left) x->parent->left = y;  // if x = a left child
  else x->parent->right = y;  // if x = a right child

  y->right = x;
  x->parent = y;
}

/**
 * Fix red-black properties
 *
 *            -->    Case 1      -->      Case 2      -->      Case 3
 *      11             11                   11                   7
 *     /  \           /  \                 /  \                /   \
 *    2    14        2    14(y)           7    14(y)       2(z)     11
 *   / \     \      / \       \          / \       \      /   \    /  \
 *  1   7     15   1   7(z)    15     2(z)  8       15   1     5  8    14
 *     / \            /   \          /    \                      /       \
 *    5   8(y)       5     8        1      5                    4         15
 *   /              /                     /
 *  4(z)           4                     4
 */
void rbtree_insert_fix(rbtree_t *T, rbtree_t *z) {
  // z is RED
  while (z->parent->color == RED) {
    if (z->parent == z->parent->parent->left) {  // if 5 = a left child
      rbtree_t *y = z->parent->parent->right;  // y = sibling of z's parent
	  
      if (y->color == RED) {
        z->parent->color = BLK;                     // case 1: set 5 to black
        y->color = BLK;                             // case 1: set 8 to black
        z->parent->parent->color = RED;             // case 1: set 7 to red
        z = z->parent->parent;                      // case 1: z = 7
      }
      else if (z == z->parent->right) {  // if z = a right child and y is black
        z = z->parent;                              // case 2: z = 2
        rbtree_left_rotate(T, z);                   // case 2: left rotate on 2

        z->parent->color = BLK;                     // case 3: set 7 to black
        z->parent->parent->color = RED;             // case 3: set 11 to red
        rbtree_right_rotate(T, z->parent->parent);  // case 3: left rotate on 11
      }
      else {
        z->parent->color = BLK;                     // case 3: set 7 to black
        z->parent->parent->color = RED;             // case 3: set 11 to red
        rbtree_right_rotate(T, z->parent->parent);  // case 3: left rotate on 11
      }
    }
    else {  // same as if clause with "right" and "left" exchanged
      rbtree_t *y = z->parent->parent->left;  // y = sibling of z's parent

      if (y->color == RED) {
        z->parent->color = BLK;
        y->color = BLK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->left) {
        z = z->parent;
        rbtree_right_rotate(T, z);

        z->parent->color = BLK;
        z->parent->parent->color = RED;
        rbtree_left_rotate(T, z->parent->parent);
      }
      else {
        z->parent->color = BLK;
        z->parent->parent->color = RED;
        rbtree_left_rotate(T, z->parent->parent);
      }
    }
  }  // end of while
  T->root->color = BLK;
}

/**
 * Insert node
 * New node has color red and black-height 1
 */
rbtree_t *rbtree_insert(rbtree_t *T, rbtree_t *z) {
  rbtree_t *y = T->nil;
  rbtree_t *x = T->root;

  // empty tree never enters while: y = T->nil
  while (x != T->nil) {  // traverse down to a leaf
    y = x;
    if (z->key < x->key) x = x->left;  // traverse left subtree
    else x = x->right;  // traverse right subtree
  }
  z->parent = y;  // y stored the parent of x = nil upon loop exit
  if (y == T->nil) T->root = z;  // z is the first node
  else if (z->key < y->key) y->left = z;  // z = a left child
  else y->right = z;  // z = a right child

  rbtree_insert_fix(T, z);  // restore the red-black properties

  return z;
}

void rbtree_transplant(rbtree_t *T, rbtree_t *u, rbtree_t *v) {
  if (u->parent == T->nil) T->root = v;
  else if (u == u->parent->left) u->parent->left = v;
  else u->parent->right = v;

  v->parent = u->parent;
}

rbtree_t *rbtree_minimum(rbtree_t *T, rbtree_t *x) {
  while (x->left != T->nil) x = x->left;

  return x;
}

void rbtree_delete_fix(rbtree_t *T, rbtree_t *x) {
  while (x != T->root && x->color == BLK) {
    if (x == x->parent->left) {
      rbtree_t *w = x->parent->right;
      if (w->color == RED) {
        w->color = BLK;                    // case 1
        x->parent->color = RED;            // case 1
        rbtree_left_rotate(T, x->parent);  // case 1
        w = x->parent->right;              // case 1
      }
      if (w->left->color == BLK && w->right->color == BLK) {
        w->color = RED;                    // case 2
        x = x->parent;                     // case 2
      }
      else if (w->right->color == BLK) {
        w->left->color = BLK;              // case 3
        w->color = RED;                    // case 3
        rbtree_right_rotate(T, w);         // case 3
        w = x->parent->right;              // case 3

        w->color = x->parent->color;       // case 4
        x->parent->color = BLK;            // case 4
        w->right->color = BLK;             // case 4
        rbtree_left_rotate(T, x->parent);  // case 4
        x = T->root;                       // case 4
      }
      else {
        w->color = x->parent->color;       // case 4
        x->parent->color = BLK;            // case 4
        w->right->color = BLK;             // case 4
        rbtree_left_rotate(T, x->parent);  // case 4
        x = T->root;                       // case 4
      }
    }
    else {  // same as if clause with "right" and "left" exchanged
      rbtree_t *w = x->parent->left;
      if (w->color == RED) {
        w->color = BLK;
        x->parent->color = RED;
        rbtree_right_rotate(T, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == BLK && w->left->color == BLK) {
        w->color = RED;
        x = x->parent;
      }
      else if (w->left->color == BLK) {
        w->right->color = BLK;
        w->color = RED;
        rbtree_left_rotate(T, w);
        w = x->parent->left;

        w->color = x->parent->color;
        x->parent->color = BLK;
        w->left->color = BLK;
        rbtree_right_rotate(T, x->parent);
        x = T->root;
      }
      else {
        w->color = x->parent->color;
        x->parent->color = BLK;
        w->left->color = BLK;
        rbtree_right_rotate(T, x->parent);
        x = T->root;
      }
    }
  }  // end of while
  x->color = BLK;
}

void rbtree_delete(rbtree_t *T, rbtree_t *z) {
  rbtree_t *x;
  rbtree_t *y = z;
  int y_orig_color = y->color;

  if (z->left == T->nil) {
    x = z->right;
    rbtree_transplant(T, z, z->right);
  }
  else if (z->right == T->nil) {
    x = z->left;
    rbtree_transplant(T, z, z->left);
  }
  else {
    y = rbtree_minimum(T, z->right);
    y_orig_color = y->color;
    x = y->right;
    if (y->parent == z) x->parent = y;
    else {
      rbtree_transplant(T, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rbtree_transplant(T, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (y_orig_color == BLK) rbtree_delete_fix(T, x);
}

int rbtree_color(rbtree_t *rbt) {
  if (!rbt) return BLK;
  else return rbt->color;
}

/**
 * Code from Professor Xi
 *
 * [rbtree_check] checks whether [rbt] meets all the
 * required constraints for being a red-black tree.
 *
 * 1. the black height of [rbt] is returned if [rbt]
 *    is a valid red-black tree
 * 2. -1 is returned if [rbt] is not a valid red-black
 *    tree
 */
int rbtree_check(rbtree_t *T, rbtree_t *rbt) {
  int key;
  int l_ret, r_ret;
  rbtree_t *l_rbt, *r_rbt;

  if (rbt == T->nil) return 0;

  key = rbt->key;
  l_rbt = rbt->left;
  r_rbt = rbt->right;

  if (l_rbt != T->nil) {
    if (l_rbt->parent != rbt) return -1;   
    if (l_rbt->key > key) return -1;  // ill-ordered
  }

  if (r_rbt != T->nil) {
    if (r_rbt->parent != rbt) return -1;   
    if (r_rbt->key < key) return -1;  // ill-ordered
  }

  if (rbt->color == RED) {
    if (rbtree_color(l_rbt) == RED) return -1;
    if (rbtree_color(r_rbt) == RED) return -1;
  }

  l_ret = rbtree_check(T, l_rbt);
  if (l_ret < 0) return l_ret;

  r_ret = rbtree_check(T, r_rbt);
  if (r_ret < 0) return r_ret;

  if (l_ret != r_ret) return -1;

  if (rbt->color == BLK) return 1 + l_ret;  // inc only if [rbt] is black
  else return l_ret;
}
