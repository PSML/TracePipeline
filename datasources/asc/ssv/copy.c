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

ssv_t *asc_ssv_copy(ssv_t const *y)
{
    ssv_t *x;

    if ((x = asc_ssv_calloc(y->N)) == 0)
        return 0;

    if (asc_ssv_set(x, y) == 0)
        return 0;

    return x;
}
