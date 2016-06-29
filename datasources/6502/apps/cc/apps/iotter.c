#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"
#include <stdlib.h>

struct node {
  int32_t position;
  int32_t value;
  struct node *next;
};

int32_t potential(int32_t position, int32_t value)
{
  
    int32_t i=position+value;

    for (i = 0; i < 13; i++) {
    }

    return i;
}

int main()
{
  int32_t i, value, energy=0;
  struct node *head, *node;
  
  i = 0;
  head = (struct node *)malloc(sizeof(struct node));
  node = head;
  
  /* Set up the initial linked list.  */
  while (1) {
    cscanf("%"PRId32, &value);
    node->value = value;
    node->position = i;

    energy = potential(i, value);

    if (value == -1) {
      /* Terminate the linked list.  */
      node->next = 0;
      break;
    }
    i++;
    node->next = (struct node *)malloc(sizeof(struct node));
    node = node->next;
  }

  /* Search for a node with negative potential energy.  */
  for (node = head; node; node = node->next) {
    energy = potential(node->position,node->value);
    if (energy < 0) break; 
  }
  return energy;
}

