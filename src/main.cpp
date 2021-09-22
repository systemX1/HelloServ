#include <cstdio>
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <sys/epoll.h>

#include "util/util.h"
#include "echo.h"

int main(int argc, char* argv[]) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    listenfd = Open_listenfd(argv[1]);
    printf("Listening to port %d\n", atoi(argv[1]));

    int epfd = epoll_create1(0);
    printf("epoll created %d\n", epfd);
    sy::addEpollFd(epfd, listenfd, true);
    static struct epoll_event events[sy::EPOLL_SIZE];
    std::list<int> conn_list;

    while (true) {
        int epoll_event_count = epoll_wait(epfd, events, sy::EPOLL_SIZE, -1);
        if(epoll_event_count < 0)   sy::errExit("epoll wait error");
        printf("epoll_events_count = %d\n", epoll_event_count);
        for (int i = 0; i < epoll_event_count; ++i) {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd) {
                clientlen = sizeof(struct sockaddr_storage);
                connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
                Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
                            client_port, MAXLINE, 0);
                printf("Connected from %s:%s, id %d\n", client_hostname, client_port, connfd);

                sy::addEpollFd(epfd, connfd, true);
                conn_list.push_back(connfd);
                const char *msg = ". Login succ\n";

                if (send(connfd, msg, strlen(msg), 0) < 0)
                    fprintf(stderr, "send to client %d error\n", connfd);
            } else if(sy::sendBroadcastMsg(sockfd, conn_list) < 0) {
                fprintf(stderr, "send broadcast from client %d error\n", sockfd);
            }
        }
    }
    Close(epfd);
    Close(connfd);
    exit(EXIT_SUCCESS);
}
