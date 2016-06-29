#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <gmp.h>
#include <fann.h>
#include <error.h>
#include <errno.h>
#include <asc.h>

int perf_event_open(struct perf_event_attr *attr, pid_t pid,
                    int cpu, int group_fd, unsigned long flags);

int asc_command_perf(int argc, char **argv, double tic)
{
    struct perf_event_attr pe;
    double toc, dt, mips;
    uint64_t payoff;
    int fd, status;
    pid_t pid, nr;

    if ((pid = asc_ptrace_start(argc, argv)) < 0)
        return -1;

    memset(&pe, 0, sizeof(struct perf_event_attr));

    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    if ((fd = syscall(__NR_perf_event_open, &pe, pid, -1, -1, 0)) < 0) {
        printk("perf_event_open failed");
        return -1;
    }

    if (ioctl(fd, PERF_EVENT_IOC_RESET, 0)) {
        printk("ioctl failed");
        return -1;
    }

    if (ioctl(fd, PERF_EVENT_IOC_ENABLE, 0)) {
        printk("ioctl failed");
        return -1;
    }

    if (ptrace(PTRACE_CONT, pid, 0, 0) < 0) {
        printk("ptrace CONT on pid `%d' failed", pid);
        return -1;
    }

    if ((nr = waitpid(-1, &status, 0)) < 0) {
        printk("waitpid on pid `%d' failed", pid);
        return -1;
    }

    if (read(fd, &payoff, sizeof(uint64_t)) != sizeof(uint64_t)) {
        printk("read from fd %u for pid `%d' failed", fd, pid);
        return -1;
    }

    toc = posix_timestamp();

    dt = toc - tic;

    mips = payoff / dt / 1e6;

    fprintf(stderr, "%15s\t%15s\t%15s\n%15lu\t%15.9f\t%15.9f\n",
            "Instructions", "Seconds", "MIPS", payoff, dt, mips);

    return 0;
}
