#include "rules.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <dlfcn.h>

int __libc_start_main(int *(main)(int, char **, char **),
                      int argc,
                      char **argv,
                      void (*init)(void),
                      void (*fini)(void),
                      void (*rtld_fini)(void),
                      void (*stack_end)) {
    int (*libc_start_main)(int *(*)(int, char **, char **), int, char **, void (*)(), void (*)(), void (*)(),
                           void *) = (int (*)(int *(*)(int, char **, char **), int, char **, void (*)(), void (*)(),
                                              void (*)(), void *)) dlsym(dlopen("libc.so.6", RTLD_LOCAL | RTLD_LAZY),
                                                                         "__libc_start_main");
    seccomp();
    return (*libc_start_main)(main, argc, argv, init, fini, rtld_fini, stack_end);
}

void seccomp() {
    prctl(PR_SET_NO_NEW_PRIVS, 1);
    prctl(PR_SET_DUMPABLE, 0);
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
    rules(&ctx);
    seccomp_load(ctx);
}

void rules(scmp_filter_ctx *ctx) {
    for (int i = 0; i < (int) (sizeof(basic_rules) / sizeof(*basic_rules)); i++)
        seccomp_rule_add(*ctx, SCMP_ACT_ALLOW, basic_rules[i], 0);
    for (int i = 0; i < (int) (sizeof(other_rules) / sizeof(*other_rules)); i++)
        seccomp_rule_add(*ctx, SCMP_ACT_ALLOW, other_rules[i], 0);
}

#ifdef __cplusplus
}
#endif