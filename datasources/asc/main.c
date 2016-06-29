#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include <libgen.h>
#include <gmp.h>
#include <getopt.h>
#include <fcntl.h>
#include <fann.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <asc.h>

typedef struct {
    bool singlestep;
    bool blockstep;
    bool breakpoint;
    bool instructions;
    struct {
        void *addr;
        uint64_t period;
    };
    double retired;
} asc_mode_t;

static volatile sig_atomic_t asc_target_pid;

static void handler(int nr)
{
    if (ptrace(PTRACE_SYSEMU_SINGLESTEP, asc_target_pid, 0, 0) < 0) {
        printk("ptrace SINGLESTEP failed");
        __builtin_trap();
    }
}

int main(int argc, char *argv[])
{
    double a, loss, tic, transient = 1, workers = 1, Logloss = 0, T = INFINITY;
    char net[PATH_MAX] = {0}, train[PATH_MAX] = {0};
    ssv_t *x, *y, *z, *r, *u, *v, *w, *e = 0;
    asc_mode_t *mode = (asc_mode_t []){{0}};
    char path[PATH_MAX], name[NAME_MAX];
    struct fann_train_data D = {0};
    uint64_t payoff, Payoff = 0;
    int c, status, fd[5] = {0};
    struct fann *h = 0;
    pid_t pid, wid;
    long nr, t = 0;

    /* Record our start time.  */
    tic = posix_timestamp();

    /* Dispatch on command if extant.  */
    if (argc == 1) {
        asc_help();
        goto EXIT;
    } else if (strstr("maps", argv[1])) {
        return asc_command_maps(argc - 2, argv + 2, tic);
    } else if (strstr("perf", argv[1])) {
        return asc_command_perf(argc - 2, argv + 2, tic);
    } else if (strstr("train", argv[1])) {
        return asc_command_train(argc - 1, argv + 1, tic);
    }

    /* Long command line options.  */
    struct option switches[] = {
        {"address",      required_argument, 0, 'a'},
        {"blockstep",          no_argument, 0, 'b'},
        {"help",               no_argument, 0, 'h'},
        {"instructions", required_argument, 0, 'i'},
        {"jobs",         required_argument, 0, 'j'},
        {"rounds",       required_argument, 0, 'n'},
        {"singlestep",         no_argument, 0, 's'},
        {"transient",    required_argument, 0, 't'},
        {"version",            no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    /* Short command line options.  */
    char const *letters = "+a:bhi:j:n:st:v";

    /* Parse switched arguments.  */
    while ((c = getopt_long(argc, argv, letters, switches, 0)) != -1) {
        switch (c) {
        case 'a':
            if (sscanf(optarg, "%p/%lg", &mode->addr, &a) == 2) {
                /* Both the breakpoint and period supplied.  */
                mode->breakpoint = 1;
                mode->period = a;
            } else if (sscanf(optarg, "%p", &mode->addr) == 1) {
                /* Just the breakpoint supplied.  */
                mode->breakpoint = 1;
                mode->period = 1;
            } else {
                error(0, errno = EINVAL, "-%c `%s'", c, optarg);
                goto EXIT;
            }
            break;
        case 'b':
            mode->blockstep = 1;
            break;
        case 'j':              /* Number of jobs.  */
            if (sscanf(optarg, "%lg", &a) != 1) {
                error(0, errno = EINVAL, "-%c `%s'", c, optarg);
                goto EXIT;
            }
            workers = a - 1;
            break;
        case 'h':
            asc_help();
            goto EXIT;
        case 'i':
            if (sscanf(optarg, "%lg", &mode->retired) != 1) {
                error(0, errno = EINVAL, "-%c `%s'", c, optarg);
                goto EXIT;
            }
            mode->instructions = 1;
            break;
        case 'n':              /* Upper bound on number of rounds.  */
            if (sscanf(optarg, "%lg", &T) != 1) {
                error(0, errno = EINVAL, "-%c `%s'", c, optarg);
                goto EXIT;
            }
            break;
        case 's':
            mode->singlestep = 1;
            break;
        case 't':
            if (sscanf(optarg, "%lg", &transient) != 1) {
                error(0, errno = EINVAL, "-%c `%s'", c, optarg);
                goto EXIT;
            }
            break;
        case 'v':              /* Print version string and exit.  */
            puts(VERSION);
            goto EXIT;
        default:               /* Illegal option.  */
            errno = EINVAL;
            goto EXIT;
        }
    }

    /* Parse non-switch arguments.  */
    for ( ; optind < argc; optind++) {
        if (access(argv[optind], R_OK) == 0) {
            char suffix[NAME_MAX];

            snprintf(path, PATH_MAX, "%s", argv[optind]);
            snprintf(name, NAME_MAX, "%s", basename(path));

            if (access(argv[optind], X_OK) == 0)
                break;

            if (sscanf(name, "%*[^.].%[^.]", suffix) == 1) {
                if (strcmp(suffix, "net") == 0) {
                    snprintf(net, PATH_MAX, "%s", path);
                } else if (strcmp(suffix, "train") == 0) {
                    snprintf(train, PATH_MAX, "%s", path);
                } else {
                    printk("unrecognized suffix");
                    goto PANIC;
                }
            } else {
                printk("unrecognized file");
                goto PANIC;
            }
        } else {
            printk("warning: could not find file `%s'", argv[optind]);
        }
    }

  START:
    if ((wid = asc_ptrace_start(argc - optind, argv + optind)) < 0)
        goto EXIT;
    if ((pid = asc_ptrace_start(argc - optind, argv + optind)) < 0)
        goto EXIT;
  COUNTERS:
    if ((fd[0] = asc_perf_instructions(pid, mode->retired)) < 0)
        goto PANIC;
    if ((fd[1] = asc_perf_instructions(wid, mode->retired)) < 0)
        goto PANIC;
  EVENTS:
    if (mode->breakpoint) {
        if ((fd[2] = asc_perf_breakpoint(pid, mode->addr, mode->period)) < 0)
            goto PANIC;
        if ((fd[3] = asc_perf_breakpoint(wid, mode->addr, mode->period)) < 0)
            goto PANIC;
    }
    if (mode->breakpoint && mode->instructions) {
        if (ioctl(fd[2], PERF_EVENT_IOC_DISABLE, 0) < 0)
            goto PANIC;
    }
  ALLOCATE:
    if ((x = (asc_ssv_gather(0, pid))) == 0)
        goto PANIC;
    if ((y = (asc_ssv_gather(0, pid))) == 0)
        goto PANIC;
    if ((z = (asc_ssv_gather(0, pid))) == 0)
        goto PANIC;
    if ((e = (asc_ssv_gather(0, pid))) == 0)
        goto PANIC;
    if ((r = (asc_ssv_gather(0, pid))) == 0)
        goto PANIC;
  CLEAR:
    asc_ssv_xor(x, x, x);
    asc_ssv_xor(z, z, z);
    asc_ssv_xor(e, e, e);
    if (strlen(net) > 0)
        if ((h = asc_online_resume(e, net)) == 0)
            goto PANIC;
    if (strlen(train) > 0)
        __builtin_trap();
  KICKOFF:
    if ((u = (asc_ssv_gather(0, wid))) == 0)
        goto PANIC;
    if ((v = (asc_ssv_gather(0, wid))) == 0)
        goto PANIC;
    if ((w = (asc_ssv_gather(0, wid))) == 0)
        goto PANIC;
    assert(asc_ssv_cmp(u, v) == 0);
    assert(asc_ssv_cmp(v, w) == 0);
    if (asc_ssv_cmp(u, y) != 0)
        printk("warning: auxv random bytes were not cleared");
    asc_ssv_xor(u, u, u);
    asc_ssv_xor(r, u, y);
    if (workers > 0) {
        if (ptrace(PTRACE_SYSEMU_SINGLESTEP, wid, 0, 0) < 0)
            goto PANIC;
    }
    goto RESIDUAL;

  WAIT:
    if ((nr = waitpid(-1, &status, 0)) < 0) {
        printk("error: waitpid");
        goto PANIC;
    }
  DISPATCH:
    if (WSTOPSIG(status) == (SIGTRAP | 0x80)) {
        /* This is a TRACESYSGOOD event.  */
        if (nr == pid)
            goto SYSCALL;
        else if (nr == wid) {
            goto WORKER;
        } else {
            __builtin_trap();
        }
    } else if (WSTOPSIG(status) == SIGTRAP) {
        if ((status >> 8) == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) {
            /* This is a TRACEEXIT event.  */
            if (nr == pid)
                goto SAVE;
            else if (nr == wid) {
                printk("lost our worker!");
                goto PANIC;
            } else {
                goto PANIC;
            }
        } else {
            /* This is just a normal trace trap.  */
            if (nr == pid)
                goto SWAP;
            else if (nr == wid) {
                goto WORKER;
            } else {
                goto PANIC;
            }
        }
    } else if (WSTOPSIG(status) == SIGSTOP) {
        /* This is probably a perf event.  */
        if (nr == pid)
            goto SWAP;
        else if (nr == wid) {
            goto WORKER;
        } else {
            goto PANIC;
        }
    } else if (WSTOPSIG(status) == SIGSEGV) {
        if (nr == pid) {
            /* This is bad.  */
            printk("panic: pid `%lu' SIGSEGV: stack smashed?", nr);
            goto PANIC;
        } else {
            /* This is probably a wild speculation.  */
            printk("warning: worker pid `%lu' SIGSEGV: bad speculation?", nr);
            asc_target_pid = wid;
            if (signal(SIGALRM, handler) == SIG_ERR)
                goto PANIC;
            if (alarm(1))
                goto PANIC;
            goto WAIT;
        }
    } else if (WSTOPSIG(status) == SIGWINCH) {
        printk("ignoring window size change");
        if (ptrace(PTRACE_SYSEMU, nr, 0, 0) < 0)
            goto PANIC;
        goto WAIT;
    } else {
        if (nr == wid) {
            /* This is probably just a wild speculation.  */
            printk("bad speculation?");
            goto WORKER;
        } else if (nr == pid) {
            printk("panic: got unexpected signal for pid `%lu'", nr);
            goto PANIC;
        } else {
            printk("panic: got signal from unknown pid `%lu'", nr);
            goto PANIC;
        }
    }
  SWAP:
    asc_ssv_swap(x, y);
  GATHER:
    if (mode->breakpoint && mode->instructions) {
        if (ioctl(fd[0], PERF_EVENT_IOC_DISABLE, 0) < 0)
            goto PANIC;
        while ((void *) asc_ptrace_pc(pid) != mode->addr) {
            if (ptrace(PTRACE_SYSEMU_SINGLESTEP, pid, 0, 0) < 0)
                goto PANIC;
            if (waitpid(pid, &status, 0) < 0)
                goto PANIC;
        }
        if (ioctl(fd[0], PERF_EVENT_IOC_ENABLE, 0) < 0)
            goto PANIC;
    }
    if ((asc_ssv_gather(y, pid)) == 0)
        goto PANIC;
  RESIDUAL:
    asc_ssv_xor(r, u, y);
  LOSS:
    /* Calculate 0/1-loss.  */
    if ((loss = asc_ssv_popcount(r)) == 0)
        goto TUNNEL;
  TRANSITION:
    if (mode->singlestep) {
        if (ptrace(PTRACE_SYSEMU_SINGLESTEP, pid, 0, 0) < 0)
            goto PANIC;
    } else if (mode->blockstep) {
        if (ptrace(PTRACE_SINGLEBLOCK, pid, 0, 0) < 0)
            goto PANIC;
    } else if (mode->breakpoint && mode->instructions) {
        if (ptrace(PTRACE_SYSEMU, pid, 0, 0) < 0)
            goto PANIC;
    } else {
        if (ptrace(PTRACE_SYSEMU, pid, 0, 0) < 0)
            goto PANIC;
    }
    goto DIFF;
  TUNNEL:
    Payoff += payoff;
    if (asc_ssv_scatter(pid, v))
        goto PANIC;
  DIFF:
    /* Skip on-line learning if `x' to `y' is not an example of return map.  */
    if (mode->breakpoint) {
        if ((void *) asc_ssv_pc(x) != mode->addr) {
            goto READOUT;
        }
        if ((void *) asc_ssv_pc(y) != mode->addr) {
            goto READOUT;
        }
    }
    asc_ssv_xor(z, x, y);
  EXCITE:
    if (transient > 0 && t > transient)
        asc_ssv_ior(e, e, z);
  UPDATE:
    h = asc_online_update(h, &D, z, x, e);
    /* Calculate cross-entropy loss.  */
    Logloss += asc_online_loss(z, x, h, e);
  READOUT:
#if 0
    #include "display/recurrence.h"
#else
    #include "display/default.h"
#endif
  LOOP:
    if (t++ >= T)
        goto SAVE;
    if (loss == 0)
        goto SWAP;
    goto WAIT;

  SYSCALL:
    asc_ssv_swap(x, y);
    if ((asc_ssv_gather(y, pid)) == 0)
        goto PANIC;
    /* Syscall number is in RAX.
     * Arguments are in RDI, RSI, RDX, R10, R8 and R9.
     * Our return code should be placed in RAX.
     */

    long rdi = asc_ssv_qword(y, RDI);
    long rsi = asc_ssv_qword(y, RSI);
    long rdx = asc_ssv_qword(y, RDX);
#if 0
    error(0, 0, "RDI = %ld RSI = %lx RDX = %ld", rdi, rsi, rdx);
#endif
    switch (nr = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, 0)) {
    case __NR_exit:
        T = t;
        goto READOUT;
    case __NR_write:
        /* We only support writing to stdout at the moment.  */
        if (rdi != 1) {
            error(0, ENOSYS, "write to fd %ld", rdi);
            goto EXIT;
        }
        long i;
        for (i = 0; i < rdx; i++) {
            uint8_t byte = asc_ssv_byte(y, rsi + i);
#if 0
            error(0, 0, "%x `%c'\n", byte, byte);
#endif
            if (fd[4] == 0) {
                mode_t perms = S_IRUSR | S_IWUSR;
                char output[NAME_MAX];

                sprintf(output, "%s.stdout", name);

                if ((fd[4] = open(output, O_CREAT|O_WRONLY, perms)) < 0) {
                    printk("open `%s'", output);
                    goto PANIC;
                }
            }

            if (write(fd[4], &byte, 1) != 1) {
                printk("write");
                goto PANIC;
            }
        }
        if (ptrace(PTRACE_POKEUSER, pid, 8 * RAX, rdx) < 0)
            goto PANIC;
        if (ptrace(PTRACE_SYSEMU, pid, 0, 0) < 0)
            goto PANIC;
        asc_ssv_set(y, x);
        goto WAIT;
    default:
        error(0, ENOSYS, "system call `%ld'", nr);
        goto EXIT;
    }

  WORKER:
    if (h == 0 || fann_get_MSE(h) > 0.1) {
        /* Too risky.  */
        asc_target_pid = wid;
        if (signal(SIGALRM, handler) == SIG_ERR)
            goto PANIC;
        if (alarm(1)) {
            printk("stepped on existing alarm");
            goto PANIC;
        }
        goto WAIT;
    }
    if (mode->breakpoint && mode->instructions) {
        if (ioctl(fd[3], PERF_EVENT_IOC_ENABLE, 0) < 0)
            goto PANIC;
        if (ptrace(PTRACE_SYSEMU, wid, 0, 0) < 0)
            goto PANIC;
        if (waitpid(wid, &status, 0) != wid)
            goto PANIC;
        if (ioctl(fd[3], PERF_EVENT_IOC_DISABLE, 0) < 0)
            goto PANIC;
    }
    /* Set cache entry origination state.  */
    asc_ssv_set(u, w);
    /* Set cache entry destination state.  */
    if ((asc_ssv_gather(v, wid)) == 0)
        goto PANIC;
    /* Determine utility of cache entry.  */
    if (read(fd[1], &payoff, sizeof(uint64_t)) != sizeof(uint64_t))
        goto PANIC;
    /* Sanity check the cache entry.  */
    if (asc_ssv_cmp(v, w) == 0) {
        /* Invalidate cache entry if no forward progress.  */
        asc_ssv_xor(u, u, u);
        payoff = 0;
    } else if (mode->breakpoint) {
        /* Invalidate cache entry if origination program counter is wrong.  */
        if ((void *) asc_ssv_pc(u) != mode->addr) {
            asc_ssv_xor(u, u, u);
            payoff = 0;
        }

        /* Invalidate cache entry if destination program counter is wrong.  */
        if ((void *) asc_ssv_pc(v) != mode->addr) {
            asc_ssv_xor(u, u, u);
            payoff = 0;
        }
    }
    assert(asc_ssv_cmp(u, v) != 0);
  PREDICT:
    /* Now we use w to start a new tunnel. */
    asc_online_predict(w, y, e, h);
    asc_online_predict(w, w, e, h);
    if (asc_ssv_scatter(wid, w))
        goto PANIC;
  SPECULATE:
    if (ioctl(fd[1], PERF_EVENT_IOC_RESET, 0) < 0) {
        printk("error: ioctl");
        goto PANIC;
    }
    if (mode->singlestep) {
        if (ptrace(PTRACE_SYSEMU_SINGLESTEP, wid, 0, 0) < 0)
            goto PANIC;
    } else if (mode->blockstep) {
        if (ptrace(PTRACE_SINGLEBLOCK, wid, 0, 0) < 0)
            goto PANIC;
    } else if (mode->breakpoint && mode->instructions) {
        if (ptrace(PTRACE_SYSEMU, wid, 0, 0) < 0)
            goto PANIC;
    } else {
        if (ptrace(PTRACE_SYSEMU, wid, 0, 0) < 0)
            goto PANIC;
    }
    goto WAIT;

  SAVE:
    if (h) {
        if (strlen(net) == 0)
            sprintf(net, "%s.net", name);
        if (asc_online_save(net, h, e) < 0)
            goto PANIC;
    }
    if (D.num_data > 0) {
        if (strlen(train) == 0)
            sprintf(train, "%s.train", name);
        if (fann_save_train_safe(train, &D) < 0)
            goto PANIC;
    }
    if (fd[4] > 0) {
        if (close(fd[4]) < 0) {
            printk("close");
            goto PANIC;
        }
    }
    goto EXIT;

  PANIC:
    printk("PANIC: run aborted");
  EXIT:
    exit(errno);
}
