#include <gsl/gsl_randist.h>
#include <math.h>

double gsl_ran_bernoulli_lgpdf(unsigned int k, double p)
{
    return log2(gsl_ran_bernoulli_pdf(k, p)); 
}
