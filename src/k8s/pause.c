//
// Created by 张俭 on 2021/4/19.
//
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define STRINGIFY(x) #x
#define VERSION_STRING(x) STRINGIFY(x)

#ifdef VERSION
#define VERSION HEAD
#endif

static void sigdown(int signo) {
    psignal(signo, "Shutting down, got signal");
}

static void sigreap(int signo) {
    while (waitpid(-1, NULL, WHOHANG) > 0)
        ;
}

int main(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-v")) {
            printf("pause.c %s\n", VERSION_STRING(VERSION));
            return 0;
        }
    }

    if (getpid() != 1) {
        
    }
}