/* Dense region of sparse state vector.  */
typedef struct ssv_subvector {
    struct iovec iov;
    int (*peek)(struct iovec *local, struct iovec *remote, pid_t pid);
    int (*poke)(struct iovec *local, struct iovec *remote, pid_t pid);
    mpz_t z;
} ssv_sub_t;

/* Sparse state vector.  */
typedef struct ssv {
    long N;
    struct {
        uint8_t SHA[256];
        char sha[1][8];
        long uid;
    };
    ssv_sub_t sub[];
} ssv_t;

/* Error message macro.  */
#define printk(...)                                           \
do {                                                          \
    error_at_line(0, errno, __FILE__, __LINE__, __VA_ARGS__); \
} while (0)

/* Short alias for error message macro.  */
#define qq printk

/* Debugging macro.  */
#define tr() printk("%s", __func__)

#ifndef PTRACE_SYSEMU
#define PTRACE_SYSEMU            31
#endif
#ifndef PTRACE_SYSEMU_SINGLESTEP
#define PTRACE_SYSEMU_SINGLESTEP 32
#endif
#ifndef PTRACE_SINGLEBLOCK
#define PTRACE_SINGLEBLOCK       33
#endif

int asc_help(void);
ssv_t *asc_ssv_calloc(long N);
ssv_t *asc_ssv_free(ssv_t *x);
pid_t asc_ptrace_start(int argc, char *argv[]);
ssv_t *asc_ssv_gather(ssv_t *x, pid_t pid);
int asc_peek_gpr(struct iovec *local, struct iovec *remote, pid_t pid);
int asc_poke_gpr(struct iovec *local, struct iovec *remote, pid_t pid);
int asc_peek_fpr(struct iovec *local, struct iovec *remote, pid_t pid);
int asc_poke_fpr(struct iovec *local, struct iovec *remote, pid_t pid);
int asc_peek_region(struct iovec *local, struct iovec *remote, pid_t pid);
int asc_poke_region(struct iovec *local, struct iovec *remote, pid_t pid);
ssv_t *asc_ssv_bind(ssv_t *x, pid_t pid);
long asc_ssv_qword(ssv_t const *x, long i);
long asc_ssv_pc(ssv_t const *x);
int asc_ssv_swap(ssv_t *x, ssv_t *y);
int asc_maps_parse(struct iovec *iov, mode_t *mode, char **name,
                   int M, pid_t pid);
ssv_t *asc_ssv_xor(ssv_t *z, ssv_t const *x, ssv_t const *y);
long asc_ssv_popcount(ssv_t const *x);
int asc_perf_attach(struct perf_event_attr *pe, pid_t pid);
int asc_perf_read(uint64_t *counter, int fd);
int asc_ptrace_breakpoint(pid_t pid, void *addr, double period,
                          struct perf_event_attr *pe, int fd);
mp_bitcnt_t asc_ssv_scan1(ssv_t const *x, mp_bitcnt_t j);
int asc_ssv_frees(ssv_t *X[]);
long asc_ssv_cmp(ssv_t const *x, ssv_t const *y);
mp_bitcnt_t asc_ssv_hi(ssv_t const *x, int i);
ssv_t *asc_ssv_set(ssv_t *x, ssv_t const *y);
int asc_ptrace_transition(pid_t pid, void *addr, double period, int fd);
ssv_t asc_ssv_hash(ssv_t const *x);
int asc_ptrace_blockstep(pid_t pid, int *status);
int asc_ptrace_syscall(pid_t pid, int status);
ssv_t *asc_ssv_ior(ssv_t *z, ssv_t const *x, ssv_t const *y);
int asc_event(pid_t pid[], ssv_t *X[], long L[], double A[], struct fann *h[],
              int (*f[])(pid_t [], ssv_t *[], long [], double []));
mp_bitcnt_t asc_ssv_lo(ssv_t const *x, int i);
mp_bitcnt_t asc_ssv_mod(mp_bitcnt_t j, ssv_t const *x, int i);
int asc_ssv_tstbit(ssv_t const *z, mp_bitcnt_t j);
struct fann_train_data *
fann_append_train_data(struct fann_train_data *, struct fann_train_data *);
struct fann *asc_update(struct fann *h, struct fann_train_data *D,
                        ssv_t const *z, ssv_t const *x, ssv_t const *e);
double asc_predict(ssv_t *u, ssv_t const *y, ssv_t const *e, struct fann *h);
int asc_ssv_combit(ssv_t *z, mp_bitcnt_t j);
int fann_save_safe(char const *path, struct fann *net);
ssv_t *asc_ssv_setbit(ssv_t *z, mp_bitcnt_t j);
int asc_ptrace_singlestep(pid_t pid, double period, int *status);
int asc_ptrace_baseline(pid_t pid);
double asc_online_predict(ssv_t *u, ssv_t const *y,
                          ssv_t const *e, struct fann *h);
double asc_util_timestamp(void);
ssv_t *asc_ssv_copy(ssv_t const *x);
int asc_perf_breakpoint(pid_t pid, void *address, uint64_t period);
int asc_perf_instructions(pid_t pid, uint64_t period);
struct fann *asc_online_resume(ssv_t *e, char const *path);
int asc_ssv_scatter(pid_t pid, ssv_t const *x);
struct fann *asc_online_update(struct fann *h, struct fann_train_data *D,
                               ssv_t const *z, ssv_t const *x, ssv_t const *e);
uint8_t asc_ssv_byte(ssv_t const *z, long q);
int asc_online_save(char const *path, struct fann *h, ssv_t const *e);
int fann_save_train_safe(char const *path, struct fann_train_data *data);
ssv_t *asc_ssv_init(ssv_t *x);
double gsl_ran_bernoulli_lgpdf(unsigned int k, double p);
double asc_online_loss(ssv_t const *z, ssv_t const *x,
                       struct fann *h, ssv_t const *e);
int asc_ptrace_determinize(pid_t pid);
int asc_command_maps(int argc, char *argv[], double tic);
int asc_command_perf(int argc, char *argv[], double tic);
int asc_command_train(int argc, char *argv[], double tic);
long asc_ptrace_pc(pid_t pid);
double posix_timestamp(void);
