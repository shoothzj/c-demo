//
// https://stackoverflow.com/questions/11208299/how-to-make-an-http-get-request-in-c-without-libcurl
// Created by 张俭 on 2021/6/1.
//
#include <arpa/inet.h>
#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <time.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

void timestamp() {
    time_t lTime;
    lTime = time(NULL);
    printf("%s", asctime(localtime(&lTime)));
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}

/**
 * So send the message, C program needs to
 * create a socket
 * lookup the IP address
 * open the socket
 * send the request
 * wait for the response
 * close the socket
 */
int main(int argc, char *argv[]) {
    timestamp();
    int port = 3000;
    char *host = "localhost";
    char *message_fmt = "GET / HTTP/1.0\r\n\r\n";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024], response[4096];

    // fill the parameters
    sprintf(message, message_fmt, "dddd", "zzzz");
    printf("Request:\n%s\n", message);

    // create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
        error("setsockopt failed\n");

    // lookup the address
    server = gethostbyname(host);
    if (server == NULL) {
        error("ERROR, no such host");
    }

    timestamp();
    // fill in the structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    in_addr_t in_addr;
    in_addr = inet_addr(inet_ntoa(*(struct in_addr *) *(server->h_addr_list)));
    if (in_addr == (in_addr_t) -1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(server->h_addr_list));
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_addr.s_addr = in_addr;

    // connect the socket
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    timestamp();

    // send the request
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }
        if (bytes == 0) {
            break;
        }
        sent += bytes;
    } while (sent < total);

    timestamp();

    // receive the response
    memset(response, 0, sizeof(response));
    received = 0;
    bytes = read(sockfd, response + received, 4096);
    if (bytes < 0) {
        error("ERROR reading response from socket");
    }
    received += bytes;

    timestamp();
    // close the socket
    close(sockfd);

    // process the response
    printf("Response:\n%s\n", response);

    return 0;
}