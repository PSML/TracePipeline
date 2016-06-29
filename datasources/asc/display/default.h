{
    double mse, hit, toc, dt, error, mips, hamming, logloss;
    static double regret, hits, oldR;
    uint64_t counter;

    if (t < 4)
        regret = loss;
    else
        regret += loss;

    mse = h ? fann_get_MSE(h) : 0.25;
    hit = (loss == 0);
    hits += hit;
    error = 100 * (1 - hits / (t + 1));
    toc = posix_timestamp();
    dt = toc - tic;

    hamming = asc_ssv_popcount(z);

    if (read(fd[0], &counter, sizeof(uint64_t)) != sizeof(uint64_t))
        __builtin_trap();

#if 0
    if (mode->breakpoint && mode->instructions) {
        static uint64_t oldcounter;

        if (t > 1) {
            if (counter - oldcounter == 0)
                printk("counter = %lu oldcounter = %lu", counter, oldcounter);

            assert(counter - oldcounter > 0);
        }
    }

    oldcounter = counter;
#endif

    double R = counter;
    double q = t + 1;

    R = MAX(1, counter - q);

    R = MAX(oldR + 1, R);

    oldR = R;

    logloss = Logloss / (t + 1);

    if (t == 0)
        mips = 0;
    else
        mips = ((R + Payoff) / 1e6) / dt;

    if (t == 0)
        printf("%7s\t%6s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\n",
               "Round", "PC", "Hash", "Excited", "Hamming", "Loss",
               "MSE", "Logloss", "Misses", "MIPS");

    printf("%7ld\t", t);

    printf("%06lx\t", asc_ssv_pc(y));

#if 1
    printf("%.7s\t", asc_ssv_hash(y).sha[0]);
#else
    printf("%.7s\t", "0000000");
#endif

    printf("%7ld\t", asc_ssv_popcount(e));

    if (hamming < 1e6)
        printf("%7.0f\t", hamming);
    else
        printf("%7.1e\t", hamming);

    if (loss < 1e7)
        printf("%7.0f\t", loss);
    else
        printf("%7.1e\t", loss);

    printf("%7.4f\t", mse);

    if (logloss < 1e-1)
        printf("%7.5f\t", logloss);
    else if (logloss < 1e+5)
        printf("%7.1f\t", logloss);
    else
        printf("%7.1e\t", logloss);

    printf(loss == 0 ? "%+7.2f\t" : "%7.2f\t", error);

    if (mips < 1)
        printf("%7.5f", mips);
    else if (mips >= 10000)
        printf("%7.1f", mips);
    else
        printf("%7.2f", mips);

    printf("\n");
    fflush(stdout);
}
