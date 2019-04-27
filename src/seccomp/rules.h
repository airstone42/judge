#ifndef RULES_H
#define RULES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <seccomp.h>
#include <sys/prctl.h>

void seccomp_add();

void rules_add(scmp_filter_ctx *ctx);

const int basic_rules[] = {SCMP_SYS(rt_sigreturn), SCMP_SYS(exit), SCMP_SYS(read), SCMP_SYS(write)};

const int other_rules[] = {SCMP_SYS(execve), SCMP_SYS(arch_prctl), SCMP_SYS(access), SCMP_SYS(openat), SCMP_SYS(fstat),
                           SCMP_SYS(mmap), SCMP_SYS(lseek), SCMP_SYS(close), SCMP_SYS(mprotect), SCMP_SYS(munmap),
                           SCMP_SYS(exit_group)};

#ifdef __cplusplus
}
#endif

#endif
