//
// Created by sy on 2021/9/18.
//

#include <cerrno>

#include "util.h"

void sy::errExit(const char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_SUCCESS);
}

void sy::addEpollFd(int epollFd, int fd, bool isEnableET)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (isEnableET) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
    setNonBlock(fd);
    printf("fd added to epoll\n\n");
}

int sy::setNonBlock(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

int sy::sendBroadcastMsg(int connfd, std::list<int>& conn_list)
{
    char buf[sy::BUF_SIZE];
    char message[sy::BUF_SIZE];
    bzero(buf, sy::BUF_SIZE);
    bzero(buf, sy::BUF_SIZE);

    //const char *SERVER_MESSAGE = " ClientID %d: %s";
    const char *SERVER_MESSAGE = "%s";
    const char *CAUTION = "Only one in the Chat Room\n";

    printf("read from client(clientID = %d)\n", connfd);
    ssize_t len = recv(connfd, buf, sy::BUF_SIZE, 0);

    if (len <= 0) {
        conn_list.remove(connfd);
        Close(connfd);
        printf("ClientID = %d left\n %d peo in the Chat Group\n",
               connfd, (int)conn_list.size());
    } else {
        if (conn_list.size() == 1) {
            //send(connfd, CAUTION, strlen(CAUTION), 0);
        }

        //sprintf(message, SERVER_MESSAGE, connfd, buf);

        sprintf(message, SERVER_MESSAGE, buf);

        for (auto it : conn_list) {
            if(it != connfd) {
                if (send(it, message, strlen(message), 0) < 0) {
                    fprintf(stderr, "broadcast error");
                    break;
                }
            }
        }
    }
    return static_cast<int>(len);
}


//int sy::sendBroadcastMsg(int connfd)
//{
//    size_t n;
//    char buf[MAXLINE];
//    rio_t rio;
//
//    Rio_readinitb(&rio, connfd);
//    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
//        printf("server received %d bytes from client %d\n", (int)n, connfd);
//        for (auto it : sy::conn_list) {
//            if (it == connfd) {
//                Rio_writen(connfd, buf, n);
//            }
//        }
//
//    }
//    return n;
//}



