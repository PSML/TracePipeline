#include <fann.h>
#include <sys/uio.h>
#include <linux/perf_event.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <asc.h>

ssv_t *asc_ssv_init(ssv_t *x)
{
    long i;

    assert(x);

    for (i = 0; i < x->N; i++)
        mpz_init(x->sub[i].z);

    return x;
}
