//
// https://github.com/nickdesaulniers/c-http-server/blob/master/http_server.c
// Created by 张俭 on 2021/6/1.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

// The port users will connect to
#define PORT "3000"
// How many pending connections queue will hold
#define BACKLOG 10

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// get sock addr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6 *) sa)->sin6_addr);
    }
}

struct ThreadData {
    int new_fd;
    char *body;
};

void *req_handler(void *td) {
    struct ThreadData *data = (struct ThreadData *) td;
    int new_fd = data->new_fd;
    const char *const msg = data->body;
    const char *const fmt_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %ld\r\n"
            "\r\n"
            "%s";
    char buf[200];
    pthread_detach(pthread_self());
    snprintf(buf, 200, fmt_header, strlen(msg), msg);

    if (send(new_fd, buf, strlen(buf), 0) == -1) {
        perror("send");
    }

    sleep(3);
    close(new_fd);
    return NULL;
}

int main() {
    // listen on sock_fd, new connection on new_fd
    int sockFd, newFd;
    struct addrinfo hints, *servInfo, *p;
    // connector's address
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    // initialize to 0
    hints = (struct addrinfo) {0};
    hints = (struct addrinfo) {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE
    };

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servInfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all results and bind to the first we can
    for (p = servInfo; p != NULL; p = p->ai_next) {
        if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
            perror("server: socket");
            continue;
        }


        if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
            perror("setsockopt");
            return 1;
        }

        if (bind(sockFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockFd);
            perror("server:bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servInfo); // all done with this struct

    if (listen(sockFd, BACKLOG) == -1) {
        perror("listen");
        return 1;
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    puts("server: waiting for connections...");
    sin_size = sizeof their_addr;

    // main accept loop
    while (1) {
        newFd = accept(sockFd, (struct sockaddr*) &their_addr, &sin_size);
        if (newFd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr*) &their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t thread;
        pthread_create(&thread, NULL, req_handler, &(struct ThreadData) {
                .new_fd = newFd,
                .body = "hello world!\n"
        });
    }

}



