#include "tdd.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

str_t  String;
part_t Partitions;
temp_t Temp;
sort_t Suffixes;
stack_t Stack;
tree_t Tree;

uint32_t BUFSIZE = 4096;

int
my_malloc(void **buf_ptr, int elem_size, uint32_t numbytes)
{
  if ((*buf_ptr = malloc(numbytes*elem_size)) == 0) {
    perror("ERROR: my_malloc: malloc");
    return -1;
  }
  memset((void *)*buf_ptr, 0, numbytes*elem_size);
  
  return 0;
}

int
my_realloc(void **list_ptr, int elem_size, uint32_t alloc, uint32_t inc)
{
  if ((*list_ptr = realloc(*list_ptr, elem_size*(alloc + inc))) == 0) {
    perror("ERROR: my_realloc: realloc");
    return -1;
  }
  memset((void *)((char *)*list_ptr + (elem_size*alloc)),
	 0, elem_size*inc);

  return 0;
}

int
partition_block(uint32_t block_size)
{
  uint32_t i;
  id_t cur;
  olist_t *tmp;
  
  for (i=0; i<block_size; i++) {
    cur = *(String.list + String.size + i);
    if (cur >= Partitions.size) Partitions.size = cur+1;

    /* increase number of Partitions if necessary */
    while (Partitions.size >= Partitions.alloc) {
      if (my_realloc((void *)&Partitions.olist, Partitions.alloc,
		     sizeof(olist_t *), BUFSIZE) < 0) return -1;
      if (my_realloc((void *)&Partitions.last, Partitions.alloc,
		     sizeof(olist_t *), BUFSIZE) < 0) return -1;
      Partitions.alloc += BUFSIZE;
    }

    /* add current id into the Partitions list */
    if (Partitions.olist[cur]) {
      tmp = (olist_t *)malloc(sizeof(olist_t *));
      tmp->offset = String.size + i;
      tmp->next = NULL;
      Partitions.last[cur]->next = tmp;
      Partitions.last[cur] = tmp;
    } else {
      Partitions.olist[cur] = (olist_t *)malloc(sizeof(olist_t *));
      Partitions.olist[cur]->offset = String.size + i;
      Partitions.olist[cur]->next = NULL;
      Partitions.last[cur] = Partitions.olist[cur];
    }
  }
  return 0;
}

void
print_String(uint32_t n)
{
  int i;
  
  DLOG("n=%" PRIu32":", n);
  for (i=0; i<n; i++)
    DLOG(" %" PRIu32, *(String.list + String.size + i));
  DLOG("\n");
}

void
print_Partitions(void)
{
  int i;
  olist_t *cur;

  DLOG("Partitions:\n");
  for (i=0; i<Partitions.size; i++) {
    cur = Partitions.olist[i];
    if (cur) {
      DLOG("  %d ->", i);
      while (cur) {
	DLOG(" %" PRId64, cur->offset);
	cur = cur->next;
      }
      DLOG("\n");
    }
  }
}

void
print_Suffixes(void)
{
  int i;
  trc_t cnt;
  olist_t *cur;

  DLOG("Suffixes:\n");
  for (i=0; i<Suffixes.size; i++) {
    cnt = Suffixes.cnts[i];
    if (cnt) {
      DLOG("  %d: %" PRId64 " ->", i, cnt);
      cur = Suffixes.olist[i];
      while (cur) {
	DLOG(" %" PRId64, cur->offset);
	cur = cur->next;
      }
      DLOG("\n");
    }
  }
}

int
get_LCP(trc_t *offset_list, trc_t list_size)
{
  int i, lcp=0;
  id_t cur;

  while (1) {
    cur = *(String.list + offset_list[0] + lcp);
    if (list_size == 1) return lcp;
    for (i=1; i<list_size; i++) {
      if (cur != *(String.list + offset_list[i] + lcp))
	return lcp;
    }
    lcp++;
    if (offset_list[0] + lcp >= String.size)
      return lcp;
  }
  return lcp;
}


void
print_list(trc_t *list, trc_t size)
{
  int i;

  for (i=0; i<size; i++)
    DLOG(" %" PRId64, list[i]);
  DLOG("\n");
}

int
populate_Temp(olist_t *list)
{
  trc_t diff, alloc;
  olist_t *cur;

  if (my_malloc((void *)&Temp.list, sizeof(off_t),
		BUFSIZE) < 0) return -1;
  Temp.size = 0; Temp.alloc = BUFSIZE;
  
  cur = list;
  while (cur) {
    Temp.list[Temp.size++] = cur->offset;
    
    /* increase the size of Temp if necessary */
    if (Temp.size >= Temp.alloc) {
      diff = Temp.size - Temp.alloc;
      alloc = (diff > BUFSIZE) ? diff : BUFSIZE;
      if (my_realloc((void *)&Temp.list, Temp.alloc,
		     sizeof(off_t), alloc) < 0) return -1;
      Temp.alloc += alloc;
    }
    cur = cur->next;
  }
  DLOG("Temp:");
  DFUNC(print_list(Temp.list, Temp.size));
  
  return 0;
}

int
sort_Temp(int lcp)
{
  id_t cur;
  trc_t i, diff, alloc;
  olist_t *tmp;

  /* advance offsets by the Longest Common Prefix */
  if (lcp) {
    for (i=0; i<Temp.size; i++)
      Temp.list[i] += lcp;
    DLOG("Temp:");
    DFUNC(print_list(Temp.list, Temp.size));
  }

  if (my_malloc((void *)&Suffixes.cnts, sizeof(off_t),
		BUFSIZE) < 0) return -1;
  if (my_malloc((void *)&Suffixes.olist, sizeof(olist_t *),
		BUFSIZE) < 0) return -1;
  if (my_malloc((void *)&Suffixes.last, sizeof(olist_t *),
		BUFSIZE) < 0) return -1;
  Suffixes.size = 0; Suffixes.alloc = BUFSIZE;
  
  /**
     count sort:
     for each id in the alphabet, we count the number
     of occurrences of that id
  */
  for (i=0; i<Temp.size; i++) {
    cur = *(String.list + Temp.list[i]);
    if (cur >= Suffixes.size) Suffixes.size = cur+1;

    /* increase the size of Suffixes if necessary */
    if (Suffixes.size > Suffixes.alloc) {
      diff = Suffixes.size - Suffixes.alloc;
      alloc = (diff > BUFSIZE) ? diff : BUFSIZE;
      if (my_realloc((void *)&Suffixes.cnts, Suffixes.alloc,
		     sizeof(trc_t), alloc) < 0) return -1;
      if (my_realloc((void *)&Suffixes.olist, Suffixes.alloc,
		     sizeof(olist_t *), alloc) < 0) return -1;
      Suffixes.alloc += alloc;
    }
    Suffixes.cnts[cur]++;
    if (Suffixes.cnts[cur] == 1) {
      Suffixes.olist[cur] = (olist_t *)malloc(sizeof(olist_t *));
      Suffixes.olist[cur]->offset = Temp.list[i];
      Suffixes.olist[cur]->next = NULL;
      Suffixes.last[cur] = Suffixes.olist[cur];
    } else {
      tmp = (olist_t *)malloc(sizeof(olist_t *));
      tmp->offset = Temp.list[i];
      tmp->next = NULL;
      Suffixes.last[cur]->next = tmp;
      Suffixes.last[cur] = tmp;
    }
  }
  DFUNC(print_Suffixes());

  free(Temp.list);
  
  return 0;
}

int
output_leaf(trc_t offset)
{
  trc_t idx = Tree.size;
  DLOG("LEAF: idx=%" PRId64 " off=%" PRId64 "\n", idx, offset);
  
  Tree.nlist[idx].type = T_LEAF;
  Tree.nlist[idx].off  = offset;
  Tree.nlist[idx].terminate = 0;

  Tree.size++;
  
  return idx;
}

int
output_branch(trc_t offset)
{
  trc_t idx = Tree.size;

  /* increase size of Tree if necessary */
  if (idx >= Tree.alloc) {
    if (my_realloc((void *)&Tree.nlist, Tree.alloc,
		   sizeof(node_t), BUFSIZE) < 0) return -1;
    Tree.alloc += BUFSIZE;
  }
  
  Tree.nlist[idx].type = T_BRANCH;
  Tree.nlist[idx].off  = offset;
  Tree.nlist[idx].terminate = 0;

  Tree.size++;
  
  return idx;
}

void
set_terminating_node(trc_t idx)
{
  assert(idx < Tree.size);

  Tree.nlist[idx].terminate = 1;
}

int
push_branch(trc_t idx, olist_t *olist)
{
  /* increase size of Stack if necessary */
  if (Stack.size >= Stack.alloc) {
    if (my_realloc((void *)&Stack.blist, Stack.alloc,
		   sizeof(branch_t), BUFSIZE) < 0) return -1;
    Stack.alloc += BUFSIZE;
  }
  Stack.blist[Stack.size].tree_idx = idx;
  Stack.blist[Stack.size].olist = olist;

  Stack.size++;

  return 0;
}

branch_t
pop_branch(void)
{
  branch_t cur = Stack.blist[Stack.size - 1];
  Stack.size--;

  return cur;
}

int
build_suffix_tree(void)
{
  int lcp, i, j, idx, cnt;
  trc_t b_idx;
  branch_t b;
  olist_t *cur, *tmp;

  idx = -1;
  for (i=0; i<Partitions.size; i++) {
    if (Partitions.olist[i]) {
      
      /* populate Temp from the current partition */
      if (populate_Temp(Partitions.olist[i]) < 0) return -1;

      /* output leaf and branch nodes to the Tree */
      if (Partitions.olist[i]->next == NULL) {
	if ((idx = output_leaf(Partitions.olist[i]->offset)) < 0)
	  return -1;
      } else {
	if ((idx = output_branch(Partitions.olist[i]->offset)) < 0)
	  return -1;

	/* push branch nodes onto the Stack */
	if (push_branch(idx, Partitions.olist[i]) < 0)
	  return -1;
      }
    }
  }
  if (idx >= 0) set_terminating_node(idx);

  while (Stack.size) {
    b = pop_branch();

    /* update branch node */
    b_idx = b.tree_idx;
    Tree.nlist[b_idx].idx = Tree.size;
    DLOG("BRANCH: idx=%" PRId64 " off=%" PRId64 " leaf=%" PRIu32 "\n",
	 b_idx, (trc_t)Tree.nlist[b_idx].off, Tree.nlist[b_idx].idx);

    /* populate Temp with the offsets */
    if (populate_Temp(b.olist) < 0) return -1;

    /* find the Longest Common Prefix of the offsets */
    lcp = get_LCP(Temp.list, Temp.size);

    /* advance offsets by the LCP and sort*/
    if (sort_Temp(lcp) < 0) return -1;

    idx = -1;
    /* output leaf and branch nodes to the Tree */
    for (j=0; j<Suffixes.size; j++) {
      cnt = Suffixes.cnts[j];
      if (cnt == 1) {
	if ((idx = output_leaf(Suffixes.olist[j]->offset)) < 0)
	  return -1;
      }
      else if (cnt > 1) {
	if ((idx = output_branch(Suffixes.olist[j]->offset)) < 0)
	  return -1;
	
	/* push branch nodes onto the Stack */
	if (push_branch(idx, Suffixes.olist[j]) < 0)
	  return -1;
      }
    }
    if (idx >= 0) set_terminating_node(idx);

    free(Suffixes.cnts);
    cur = *Suffixes.olist;
    while (cur) {
      tmp = cur;
      cur = cur->next;
      free(tmp);
    }
    free(Suffixes.last);
  }
  return 0;
}

void
print_Tree(void)
{
  trc_t i;

  fprintf(stdout, "Suffix Tree:");
  for (i=0; i<Tree.size; i++) {
    fprintf(stdout, " %" PRId64 ":%" PRId64,
	    i, (trc_t)Tree.nlist[i].off);
    if (Tree.nlist[i].type == T_BRANCH)
      fprintf(stdout, ",%" PRId32, Tree.nlist[i].idx);
    if (Tree.nlist[i].terminate)
      fprintf(stdout, "R");
  }
  fprintf(stdout, "\n");
}

int
main(int argc, char **argv)
{
  uint32_t n;

  /* initialize global data structures */
  if (my_malloc((void *)&String.list, sizeof(id_t),
		10*BUFSIZE) < 0) return -1;
  String.size = 0; String.alloc = BUFSIZE;
  if (my_malloc((void *)&Partitions.olist, sizeof(olist_t *),
		BUFSIZE) < 0) return -1;
  if (my_malloc((void *)&Partitions.last, sizeof(olist_t *),
		BUFSIZE) < 0) return -1;
  Partitions.size = 0; Partitions.alloc = BUFSIZE;
  if (my_malloc((void *)&Stack.blist, sizeof(branch_t),
		BUFSIZE) < 0) return -1;
  Stack.size = 0; Stack.alloc = BUFSIZE;
  if (my_malloc((void *)&Tree.nlist, sizeof(node_t),
		BUFSIZE) < 0) return -1;
  Tree.size = 0; Tree.alloc = BUFSIZE;
  
  /* populate String and Partitions */
  if (freopen(NULL, "rb", stdin) == 0) {
    perror("ERROR: freopen");
    return -1;
  }
  while ((n = fread(String.list + String.size, sizeof(id_t),
		    BUFSIZE, stdin)) > 0) {
    DFUNC(print_String(n));
    if (partition_block(n) < 0) return -1;
    String.size += n;

    /* increase the size of String if necessary */
    while (String.size >= String.alloc) {
      if (my_realloc((void *)&String.list, sizeof(id_t),
		     String.alloc, 10*BUFSIZE) < 0) return -1;
      String.alloc += 10*BUFSIZE;
    }
  }
  DFUNC(print_Partitions());

  /* build suffix tree */
  if (build_suffix_tree() < 0) return -1;

  /* output suffix tree */
  print_Tree();
  
  return 0;
}
