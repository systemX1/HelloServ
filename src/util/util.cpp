//
// Created by util on 2021/9/18.
//

#include <cerrno>

#include "util.h"

void util::errExit(const char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_SUCCESS);
}

void util::addEpollFd(int epollFd, int fd, bool isEnableET)
{
    struct epoll_event ev{};
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (isEnableET) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
    setNonBlock(fd);
    printf("fd added to epoll\n\n");
}

int util::setNonBlock(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

int util::sendBroadcastMsg(int connfd, std::list<int>& conn_list)
{
    char buf[util::BUF_SIZE];
    char message[util::BUF_SIZE];
    bzero(buf, util::BUF_SIZE);
    bzero(buf, util::BUF_SIZE);

    const char *SERVER_MESSAGE = "%s";

    printf("read from client(clientID = %d)\n", connfd);
    ssize_t len = recv(connfd, buf, util::BUF_SIZE, 0);

    if (len <= 0) {
        conn_list.remove(connfd);
        close(connfd);
        printf("ClientID = %d left\n %d peo in the Chat Group\n",
               connfd, (int)conn_list.size());
    } else {
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

int util::acceptConn(int s, struct sockaddr *addr, socklen_t *addrLen) {
    int rc;

    if ((rc = accept(s, addr, addrLen)) < 0)
        errExit("Connect error");
    return rc;
}

void util::getNameInfo(const struct sockaddr *sa, socklen_t salen, char *host,
                       size_t hostlen, char *serv, size_t servlen, int flags)
{
    if (getnameinfo(sa, salen, host, hostlen, serv,
                          servlen, flags) != 0)
        errExit("Getnameinfo error");
}

int util::openListenFd(char *port)
{
    int rc;

    if ((rc = openListenFd_(port)) < 0)
        errExit("openListenFd error");
    return rc;
}

int util::openListenFd_(char *port)
{
    struct addrinfo hints{}, *listp, *p;
    int listenfd, rc, optval = 1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    if ((rc = getaddrinfo(nullptr, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, util::LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }
    return listenfd;
}




