{
    static ssv_t **X;
    long i, j, k;

    if ((X = realloc(X, (t + 1) * sizeof(ssv_t *))) == 0)
        goto PANIC;

    if ((X[t] = asc_ssv_copy(y)) == 0)
        goto PANIC;

    if (t >= T) {
        for (i = 0; i < t; i++) {
            for (j = 0; j < t; j++) {
                asc_ssv_xor(z, X[i], X[j]);

                k  = asc_ssv_popcount(z);

                printf("%3ld ", k);
            }
            printf("\n");
        }
    }
}
