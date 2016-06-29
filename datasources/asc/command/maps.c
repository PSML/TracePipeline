#include <fann.h>
#include <errno.h>
#include <linux/perf_event.h>
#include <error.h>
#include <gmp.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <assert.h>
#include <asc.h>

int asc_command_maps(int argc, char **argv, double tic)
{
    int const max_maps = 32;
    int i, num_maps;
    pid_t pid;

    if ((pid = asc_ptrace_start(argc, argv)) < 0)
        return -1;

    char buf[max_maps][NAME_MAX];
    struct iovec iov[max_maps];
    mode_t mode[max_maps];
    char *name[max_maps];

    for (i = 0; i < max_maps; i++)
        name[i] = buf[i];

    if ((num_maps = asc_maps_parse(iov, mode, name, max_maps, pid)) == 0)
        return -1;

    for (i = 0; i < num_maps; i++)
        printf("%23p\t%15lu\t    %c%c%c\t%s\n",
               iov[i].iov_base,
               iov[i].iov_len,
               mode[i] & S_IRUSR ? 'r' : '-',
               mode[i] & S_IWUSR ? 'w' : '-',
               mode[i] & S_IXUSR ? 'x' : '-',
               name[i]);

    return 0;
}
