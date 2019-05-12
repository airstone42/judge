#include "conf.h"

namespace judge {
    const char *const PORT = "5555";
    const char *const INPROC_ADDRESS = "inproc://worker";
    const int MAX_THREADS = 100;
    extern const int MAX_WORKS = 10;
    const int TIME_LIMIT_MS = 1000;
    const int TIME_INTERVAL_MS = 20;
    const char *const C_COMPILER = "gcc";
    const char *const CXX_COMPILER = "g++";
    const char *const NULL_PATH = "/dev/null";
    const char *const COUNT_PATH = "/tmp/judge/";
    const char *const SECCOMP_SOURCE = "src/seccomp/rules.c";
    const char *const DL = "-ldl";
    const char *const SECCOMP = "-lseccomp";
}
