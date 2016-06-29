#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define N 2010
//#define ny 8
#define ny 2

struct node {
    uint32_t spins[ny];
    struct node *next;
} nodes[N];

int32_t potential(struct node *node)
{
    int32_t sum = 0;
    int32_t temp, i, j, kx, ky;
//    int32_t order = 31;
    int32_t order = 2;
    for (i = 0; i < ny; i++) {
        for (j = 0; j < 32; j++) {
            temp = 0;
            /* To order kx and ky */
            for (kx = -order; kx < order + 1; kx++) {
                for (ky = -order; ky < order + 1; ky++) {
                    if ((kx != 0) || (ky != 0)) {
                        //printf("%d", kx);
                        //printf("%d\n", ky);
                        temp +=
                            (1 -
                             (((node->spins[(i + ny + ky) % ny] >>
                                ((j + 32 + kx) % 32)) + 1) & 0x1) * 2);
                    }
                }
            }
            /*Adding them to the total energy */
            sum -= temp * (1 - (((node->spins[i] >> j) + 1) & 0x1) * 2);
        }
    }

    return sum / 2;
}

int32_t main()
{
    int32_t i, energy, j;
    struct node *node;

    /* Set up the initial linked list.  */
    for (i = 0; i < N - 1; i++) {
        for (j = 0; j < ny; j++) {
            nodes[i].spins[j] = i + j;
        }
        nodes[i].next = &nodes[i + 1];
    }

    /* Terminate the linked list.  */
    for (j = 0; j < ny; j++) {
        nodes[N - 1].spins[j] = 0;
    }
    nodes[N - 1].next = 0;

    /* Search for a node with negative potential energy.  */
    for (node = &nodes[0]; node; node = node->next) {
        energy = potential(node);
#if 0
        printf("%d\n", energy);
        fflush(stdout);
#endif
        if (energy < -1000000000)
            break;
    }

    return energy;
}
