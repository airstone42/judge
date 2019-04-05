#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    puts("System call \"write\" is allowed.");
    puts("System call \"fork/clone\" is not allowd.");
    puts("Program will exit because of calling \"fork()\".");
    fork();
    puts("You won't see this line if seccomp works.");
    return 0;
}

