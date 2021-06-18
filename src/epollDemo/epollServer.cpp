
/**
 * @ Description  :
 * @ Version      : 1.0
 * @ Author       : koritafei(koritafei@gmail.com)
 * @ Date         : 2021-06-09 17:12:53
 * @ LastEditors  : koritafei(koritafei@gmail.com)
 * @ LastEditTime : 2021-06-12 03:30:37
 * @ FilePath     : /libevent_l/src/epollDemo/epollServer.cpp
 * @ Copyright (C) 2021 koritafei(koritafei@gmail.com). All rights reserved.
 * */

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT    (7788)
#define EPOLL_MAX_NUM  (2048)
#define BUFFER_MAX_LEN (4096)

char buffer[BUFFER_MAX_LEN];

void to_upper(char *str) {
  int i;
  for (i = 0; i < strlen(str); i++) {
    str[i] = toupper(str[i]);
  }
}

int main(int argv, char **argc) {
  int                listen_fd = 0;
  int                client_fd = 0;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  socklen_t          client_len;
  int                epfd = 0;
  struct epoll_event events, *my_event;

  // socket
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  // bind

  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port        = htons(SERVER_PORT);
  bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // listen
  listen(listen_fd, 10);

  // epoll create
  epfd = epoll_create(EPOLL_MAX_NUM);
  if (epfd < -1) {
    perror("epoll_create error");
    close(epfd);
    close(listen_fd);
    return 0;
  }

  // listenfd-> epfd
  events.events  = EPOLLIN;
  events.data.fd = listen_fd;
  if (0 > epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &events)) {
    perror("epoll_ctl add listen_fd error");
    close(epfd);
    close(listen_fd);
    return 0;
  }

  my_event =
      (struct epoll_event *)malloc(EPOLL_MAX_NUM * sizeof(struct epoll_event));

  while (1) {
    // epoll wait
    int active_fds_cnt = epoll_wait(epfd, my_event, EPOLL_MAX_NUM, -1);
    int i              = 0;
    for (i = 0; i < active_fds_cnt; i++) {
      // if fd == listen_fd
      if (my_event->data.fd == listen_fd) {
        // accept
        client_fd =
            accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (0 > client_fd) {
          perror("accept error");
          continue;
        }

        char ip[20];
        printf("new connect[%s:%d]\n",
               inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)),
               ntohs(client_addr.sin_port));
        events.events  = EPOLLIN | EPOLLET;
        events.data.fd = client_fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &events);
      } else if (my_event[i].events & EPOLLIN) {
        printf("epoll in\n");
        client_fd = my_event[i].data.fd;
        // do read
        buffer[0] = '\0';
        int n     = read(client_fd, buffer, 5);
        if (n < 0) {
          perror("read error");
          continue;
        } else if (0 == n) {
          epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, &events);
          close(client_fd);
        } else {
          printf("read buffer[%s]\n", buffer);
          buffer[n] = '\0';
#if 1
          to_upper(buffer);
          write(client_fd, buffer, strlen(buffer));
          printf("write buffer [%s]\n", buffer);
          memset(buffer, 0, BUFFER_MAX_LEN);
          events.events  = EPOLLOUT;
          events.data.fd = client_fd;
          epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &events);
#endif
        }
      } else if (my_event[i].events & EPOLLOUT) {
        printf("epoll out\n");
#if 1
        client_fd = my_event[i].data.fd;
        to_upper(buffer);
        write(client_fd, buffer, sizeof(buffer));
        printf("write buffer[%s]\n", buffer);
        events.events  = EPOLLIN;
        events.data.fd = client_fd;
        epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &events);
#endif
      }
    }
  }
}