#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/personality.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <gmp.h>
#include <fann.h>
#include <error.h>
#include <errno.h>
#include <asc.h>

pid_t asc_ptrace_start(int argc, char *argv[])
{
    struct stat buf;
    long options;
    int status;
    pid_t pid;

    /* Bail out if no kernel supplied.  */
    if (argc < 1) {
        error(0, 0, "error: missing program argument");
        errno = EINVAL;
        return -errno;
    }

    /* Bail out if kernel does not exist.  */
    if (stat(argv[0], &buf) < 0) {
        printk("stat `%s' failed", argv[0]);
        goto failure;
    }

    /* Start child process.  */
    switch (pid = fork()) {
    case -1:
        /* Bail out if error in starting child process.  */
        error(1, errno, "panic: fork failed");
        goto failure;
    case 0:
        /* Prepend dot slash if necessary.  */
        if (strchr(argv[0], '/') == 0)
            if (asprintf(&argv[0], "./%s", argv[0]) < 0)
                error(1, errno, "asprintf");

        /* Make sure arguments are delimited by a null pointer.  */
        if (argv[argc] != 0)
            error(1, 0, "argv must be zero terminated");

        /* Zero out all environment variables.  */
        if (clearenv() < 0)
            error(1, errno, "clearenv");

        /* Disable address space layout randomization.  */
        if (personality(ADDR_NO_RANDOMIZE))
            error(1, errno, "child pid `%d' panic: personality", getpid());

        /* Volunteer control over child execution to parent.  */
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
            char path[] = "/proc/sys/kernel/yama/ptrace_scope";
            FILE *stream;
            int scope;

            if ((stream = fopen(path, "r")) == 0) {
                printk("fopen `%s'", path);
                exit(1);
            }

            if (fscanf(stream, "%d", &scope) != 1)
                error(1, errno, "fscanf `%s'", path);

            if (scope > 1)
                error(1, 0, "please enable ptrace by echoing 1 to `%s'", path);

            error(1, errno, "ptrace TRACEME");
        }

        /* Replace process image with kernel.  */
        execvpe(argv[0], argv, 0);

        /* We should not be here.  */
        error(1, errno, "execvp `%s'", argv[0]);

        /* Bail out if error starting up process.  */
        exit(1);

        /* Should never be here.  */
        __builtin_trap();
    default:
        break;
    }

    /* Bail out if child process create failed.  */
    if (waitpid(pid, &status, 0) == -1) {
        printk("waitpid on child pid `%d'", pid);
        goto failure;
    }

    /* Bail out if child process is not stopped.  */
    if (WIFSTOPPED(status) == 0)
        goto failure;

    /* Ensure notification if child exits and no zombie processes.  */
    options = PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEEXIT | PTRACE_O_EXITKILL;

    /* Install process tracing options.  */
    if (ptrace(PTRACE_SETOPTIONS, pid, 0, options) < 0) {
        printk("ptrace SETOPTIONS");
        goto failure;
    }

    /* Clear sources of non-determinism.  */
    if (asc_ptrace_determinize(pid))
        goto failure;

    return pid;

  failure:
    return -1;
}
