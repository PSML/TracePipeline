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

ssv_t *asc_ssv_calloc(long N)
{
    ssv_t *x;

    assert(N > 0);

    if ((x = calloc(1, sizeof(ssv_t) + N * sizeof(ssv_sub_t))) == 0)
        goto failure;

    x->N = N;

    return asc_ssv_init(x);

  failure:
    return 0;
}
