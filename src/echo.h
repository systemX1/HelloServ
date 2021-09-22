//
// Created by sy on 2021/9/19.
//

#ifndef HELLOSERV_ECHO_H
#define HELLOSERV_ECHO_H

#include "util/util.h"

namespace sy{
    void echo(int connfd);

    void echo(int connfd)
    {
        size_t n;
        char buf[MAXLINE];
        rio_t rio;
        strcat(buf, "Welcome to HelloServ\n");

        Rio_writen(connfd, buf, strlen(buf));
        Rio_readinitb(&rio, connfd);
        while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
            printf("server received %d bytes from client %d\n", (int)n, connfd);
            Rio_writen(connfd, buf, n);
        }
    }
}

#endif //HELLOSERV_ECHO_H
