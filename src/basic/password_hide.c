//
// Created by 张俭 on 2021/4/26.
//
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i = 0;
    pid_t mypid = getpid();
    if (argc == 1)
        return 1;
    printf("argc = %d and arguments are:\n", argc);
    for (i; i < argc; i++) {
        printf("%d = %s\n", i, argv[i]);
    }
    fflush(stdout);
    sleep(30);
    printf("Replacing first argument with x:es... Now open another terminal and run: ps p %d\n", (int)mypid);
    memset(argv[1], 'x', strlen(argv[1]));
    getc(stdin);
    return 0;
}
