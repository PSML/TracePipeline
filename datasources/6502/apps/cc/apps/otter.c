#include <stdint.h>
#include <inttypes.h>
#include "cprintf.h"
#include <stdlib.h>

#define N 1112

struct node {
    int32_t position;
    struct node *next;
} nodes[N];

int32_t potential(int32_t position)
{
    int32_t i;

    for (i = 0; i < 13; i++) {
    }

    return i;
}

int32_t main()
{
    int32_t i, energy;
    struct node *node;

    /* Set up the initial linked list.  */
    for (i = 0; i < N - 1; i++) {
        nodes[i].position = i;
        nodes[i].next = &nodes[i + 1];
    }

    /* Terminate the linked list.  */
    nodes[N - 1].position = i;
    nodes[N - 1].next = 0;

    /* Search for a node with negative potential energy.  */
    for (node = &nodes[0]; node; node = node->next) {
        energy = potential(node->position);
        if (energy < 0)
            break;
    }

    return energy;
}
