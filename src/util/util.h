//
// Created by util on 2021/9/18.
//

#ifndef HELLOSOCKET_UTIL_H
#define HELLOSOCKET_UTIL_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <list>
#include <fcntl.h>

namespace util{
    const int EPOLL_SIZE = 4096;
    const int MAX_LINE = 1024;
    const int BUF_SIZE = 128;
    const int LISTENQ = 1024;

    struct conf{
        int       list_s{};                /*  listening socket          */
        int       conn_s{};                /*  connection socket         */
        short int port{};                  /*  port number               */
        struct    sockaddr_in servaddr{};  /*  socket address structure  */
        char      buffer[MAX_LINE]{};      /*  character buffer          */
        char     *endptr{};                /*  for strtol()              */
    };
    typedef sockaddr SA;

    // error handler
    void errExit(const char* msg);

    void addEpollFd(int epollFd, int fd, bool isEnableET);
    int setNonBlock(int sockfd);
    int sendBroadcastMsg(int connfd, std::list<int>& conn_list);
    int acceptConn(int s, struct sockaddr *addr, socklen_t *addrLen);
    void getNameInfo(const struct sockaddr *sa, socklen_t salen, char *host,
                     size_t hostlen, char *serv, size_t servlen, int flags);
    int openListenFd(char *port);
    int openListenFd_(char *port);
}

#endif //HELLOSOCKET_UTIL_H
