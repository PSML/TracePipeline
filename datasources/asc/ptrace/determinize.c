#include <sys/uio.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/auxv.h>
#include <linux/perf_event.h>
#include <gmp.h>
#include <fann.h>
#include <error.h>
#include <errno.h>
#include <asc.h>

int asc_ptrace_determinize(pid_t pid)
{
    uint64_t rsp, qword, addr, argp, envp, auxp, randp, key, value = 0;
    int i;

    rsp = ptrace(PTRACE_PEEKUSER, pid, 8 * RSP, 0);

    if (errno) {
        printk("ptrace PEEKUSER on pid `%u' failed", pid);
        return -1;
    }

    argp = rsp + 8;

    for (i = 0; ; i++) {
        addr = argp + 8 * i;

        qword = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

        if (errno) {
            printk("ptrace PEEKDATA on pid `%u' failed", pid);
            return -1;
        }

        if (qword == 0)
            break;
    }

    envp = addr + 8;

    for (i = 0; ; i++) {
        addr = envp + 8 * i;

        qword = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

        if (errno) {
            printk("ptrace PEEKDATA on pid `%u' failed", pid);
            return -1;
        }

        if (qword == 0)
            break;
    }

    auxp = addr + 8;

    for (i = 0; ; i += 2) {

        addr = auxp + 8 * (i + 0);

        key = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

        if (errno) {
            printk("ptrace PEEKDATA on pid `%u' failed", pid);
            return -1;
        }

        if (key == 0)
            break;

        addr = auxp + 8 * (i + 1);

        value = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

        if (errno) {
            printk("ptrace PEEKDATA on pid `%u' failed", pid);
            return -1;
        }

        if (key == AT_RANDOM)
            break;
    }

    randp = value;

    qword = 0;

    addr = randp + 0;

    if (ptrace(PTRACE_POKEDATA, pid, addr, qword) < 0) {
        printk("%s: ptrace POKEDATA", __func__);
        return -1;
    }

    addr = randp + 8;

    if (ptrace(PTRACE_POKEDATA, pid, addr, qword) < 0) {
        printk("%s: ptrace POKEDATA", __func__);
        return -1;
    }

    return 0;
}
