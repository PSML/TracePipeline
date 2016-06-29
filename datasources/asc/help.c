#include <unistd.h>

/* Usage statement.  */
static char format[] = "`asc'"
 " executes and attempts to transparently speed up its program argument\n\n"
 "Usage: asc [COMMAND] [OPTIONS] PROGRAM [ARGUMENTS]\n"
 "\n"
 "Commands:\n"
 " maps  PROGRAM [ARGUMENTS]  "
 "  Print memory maps for PROGRAM.\n"
 " perf  PROGRAM [ARGUMENTS]  "
 "  Calculate baseline instruction rate for PROGRAM.\n"
 " train [OPTIONS] NET DATA   "
 "  Batch train neural network NET on examples DATA.\n"
 "\n"
 "Options:\n"
 " -a, --address=ADDR[/PERIOD]"
 "  Observe state vector on breakpoint at ADDR.\n"
 " -b, --blockstep            "
 "  Observe state vector on branch instructions.\n"
 " -h, --help                 "
 "  Print this help statement and exit.\n"
 " -i, --instructions=PERIOD  "
 "  Observe state vector every PERIOD instructions.\n"
 " -j, --jobs=N               "
 "  Use N parallel tasks (N=1 means serial execution).\n"
 " -n, --rounds=COUNT         "
 "  Execute for at most COUNT on-line learning rounds.\n"
 " -s, --singlestep           "
 "  Observe state vector on each instruction.\n"
 " -v, --version              "
 "  Print version string and exit.\n"
 "\n"
 "The `--instructions' and `--address' options may be supplied together.\n"
 "The files `PROGRAM.net', `PROGRAM.train', and `PROGRAM.stdout' are saved.\n";

/* Print usage statement.  */
int asc_help(void)
{
    if (write(1, format, sizeof format) < 0)
        __builtin_trap();

    return 0;
}
