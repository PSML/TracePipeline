#include <sys/uio.h>
#include <sys/param.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <gmp.h>
#include <float.h>
#include <fann.h>
#include <assert.h>
#include <asc.h>

/* Return (unnormalized) cross-entropy loss: -lg p(z|x,h). */
double asc_online_loss(ssv_t const *z, ssv_t const *x,
                       struct fann *h, ssv_t const *e)
{
    mp_bitcnt_t const J = ULONG_MAX;
    mp_bitcnt_t i, j, k;
    fann_type *Pr;
    double L = 0;

    /* Bail out early if no excitations yet.  */
    if ((k = asc_ssv_popcount(e)) == 0)
        return INFINITY;

    fann_type input[k];

    memset(input, 0, k * sizeof(fann_type));

    for (j = asc_ssv_scan1(e, 0), i = 0; j < J; j = asc_ssv_scan1(e, j + 1)) {
        if (asc_ssv_tstbit(x, j))
            input[i] = 1;
        i++;
    }

    /* Run model in forward mode.  */
    Pr = fann_run(h, input);

    for (j = asc_ssv_scan1(e, 0), i = 0; j < J; j = asc_ssv_scan1(e, j + 1)) {

        Pr[i] = MAX(Pr[i], 0 + 1e-6);
        Pr[i] = MIN(Pr[i], 1 - 1e-6);

        assert(Pr[i] > 0);
        assert(Pr[i] < 1);

        L += gsl_ran_bernoulli_lgpdf(asc_ssv_tstbit(z, j), Pr[i++]);
    }

    assert(isfinite(L));

    return -L;
}
