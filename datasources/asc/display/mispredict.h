{
    mp_bitcnt_t j;

    for (j = asc_ssv_scan1(e, 0); j < ULONG_MAX; j = asc_ssv_scan1(e, j + 1))
        if (asc_ssv_tstbit(r, j))
            fprintf(stderr, "%7lu\t%15lu\n", t, j);
}
