/**
 * @ Description  :
 * @ Version      : 1.0
 * @ Author       : koritafei(koritafei@gmail.com)
 * @ Date         : 2021-06-13 06:46:41
 * @ LastEditors  : koritafei(koritafei@gmail.com)
 * @ LastEditTime : 2021-06-14 20:10:06
 * @ FilePath     : /libevent_l/src/reactor/reactorServer.cpp
 * @ Copyright (C) 2021 koritafei(koritafei@gmail.com). All rights reserved.
 * */

#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS  1024
#define BUFLEN      128
#define SERVER_PORT 8089

/**
 * @brief
 * status :1--正在监听事件中，0--不存在
 * last_active: 最后一次响应时间，做超时处理
 * */
struct myevent_s {
  int   fd;      // cfd listenfd
  int   events;  // EPOLLIN EPOLLOUT
  void *arg;     // 指向自己的结构体指针
  void (*call_back)(int fd, int events, void *arg);
  int  status;
  char buf[BUFLEN];
  int  len;
  int  last_active;
};

int              g_efd;                     // epoll_create 返回句柄
struct myevent_s g_events[MAX_EVENTS + 1];  // 最后一个events,用于listen fd

void event_set(struct myevent_s *ev,
               int               fd,
               void (*call_back)(int, int, void *),
               void *arg) {
  ev->fd          = fd;
  ev->call_back   = call_back;
  ev->arg         = arg;
  ev->status      = 0;
  ev->last_active = time(NULL);
  ev->events      = 0;

  return;
}

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

void event_add(int efd, int events, struct myevent_s *ev) {
  struct epoll_event epv = {0, {0}};
  int                op;
  epv.data.ptr = ev;
  epv.events = ev->events = events;

  if (1 == ev->status) {
    op = EPOLL_CTL_MOD;
  } else {
    op         = EPOLL_CTL_ADD;
    ev->status = 1;
  }

  if (0 > epoll_ctl(efd, op, ev->fd, &epv)) {
    printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
  } else {
    printf("event add sucessed [fd=%d], events[%d]\n", ev->fd, events);
  }

  return;
}

void event_del(int efd, struct myevent_s *ev) {
  struct epoll_event epv = {0, {0}};
  if (1 != ev->status) {
    return;
  }

  epv.data.ptr = ev;
  ev->status   = 0;

  epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
  return;
}

void acceptconn(int lfd, int events, void *arg) {
  struct sockaddr_in cin;
  socklen_t          len = sizeof(struct sockaddr_in);
  int                cfd, i;

  if (-1 == (cfd = accept(lfd, (struct sockaddr *)&cin, &len))) {
    if (EAGAIN != errno && EINTR != errno) {
      // 暂不做处理
    }
    printf("%s: accept, %s\n", __func__, strerror(errno));
    return;
  }

  do {
    for (int i = 0; i < MAX_EVENTS; i++) {
      if (0 == g_events[i].status) {
        break;
      }
    }

    if (MAX_EVENTS == i) {
      printf("[%s], max connect limit [%d]\n", __func__, MAX_EVENTS);
      break;
    }

    int flag = 0;
    if (0 > (flag = fcntl(cfd, F_SETFL, O_NONBLOCK))) {
      printf("%s, fcntl set nonblock failed, strerror %s\n",
             __func__,
             strerror(errno));
      break;
    }

    event_set(&g_events[i], cfd, recvdata, &g_events[i]);
    event_add(g_efd, EPOLLIN, &g_events[i]);
  } while (0);

  printf("new connect [%s:%d][time:%ld], pos[%d]\n",
         inet_ntoa(cin.sin_addr),
         ntohs(cin.sin_port),
         g_events[i].last_active,
         i);

  return;
}

void recvdata(int fd, int events, void *arg) {
  struct myevent_s *ev = (struct myevent_s *)arg;
  int               len;
  len = recv(fd, ev->buf, sizeof(ev->buf), 0);
  event_del(g_efd, ev);

  if (len > 0) {
    ev->len      = len;
    ev->buf[len] = '\0';
    printf("C[%d]:%s\n", fd, ev->buf);

    // 转换为发送事件
    event_set(ev, fd, senddata, ev);
    event_add(g_efd, EPOLLOUT, ev);
  } else if (0 == len) {
    close(ev->fd);
    // ev - g_events地址相减得到偏移元素的位置
    printf("[fd=%d] pos[%d], closed\n", fd, (int)(ev - g_events));
  } else {
    close(ev->fd);
    printf("recv [fd=%d], error[%d]:%s\n", fd, strerror(errno));
  }

  return;
}

void senddata(int fd, int events, void *arg) {
  struct myevent_s *ev = (struct myevent_s *)arg;
  int               len;
  len = send(fd, ev->buf, ev->len, 0);

  printf("fd=%d\tev=->bud=%s\tev->len=%d\n", fd, ev->buf, ev->len);
  printf("send len = %d\n", len);
  event_del(g_efd, ev);

  if (len > 0) {
    printf("send[fd=%d], [%d]%s", fd, len, ev->buf);
    event_set(ev, fd, recvdata, ev);
    event_add(g_efd, EPOLLIN, ev);
  } else {
    close(ev->fd);
    printf("send [fd=%d], error %s\n", fd, strerror(errno));
  }
  return;
}

void initlistensocket(int efd, short port) {
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  fcntl(lfd, F_SETFL, O_NONBLOCK);
  event_set(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
  event_add(efd, EPOLLIN, &g_events[MAX_EVENTS]);

  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port        = htons(port);

  bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
  listen(lfd, 20);

  return;
}

int main(int argc, char **argv) {
  unsigned short port = SERVER_PORT;
  if (2 == argc) {
    port = atoi(argv[1]);
  }

  g_efd = epoll_create(MAX_EVENTS + 1);
  if (0 >= g_efd) {
    printf("create efd n %s err %s \n", __func__, strerror(errno));
  }

  initlistensocket(g_efd, port);

  // 事件循环
  struct epoll_event events[MAX_EVENTS + 1];
  printf("server running:port[%d]\n", port);
  int checkpos = 0, i;
  while (1) {
    // 超时验证，每测试100个链接，不测试listenfd当客户端60s内没有和服务器通信，则关闭此链接
    long now = time(NULL);
    for (int i = 0; i < 100; i++, checkpos++) {
      if (checkpos == MAX_EVENTS) {
        checkpos = 0;
      }
      if (g_events[checkpos].status != 1) {
        continue;
      }

      long duration = now - g_events[checkpos].last_active;
      if (60 <= duration) {
        // 超时关闭
        close(g_events[checkpos].fd);
        printf("event fd [%d] time out\n", g_events[checkpos].fd);
        event_del(g_efd, &g_events[checkpos]);
      }
    }

    // 等待事件
    int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
    if (0 > nfd) {
      printf("epoll_wait error, exits\n");
      break;
    }

    for (i = 0; i < nfd; i++) {
      struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
      if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
        ev->call_back(ev->fd, events[i].events, ev->arg);
      }

      if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
        ev->call_back(ev->fd, events[i].events, ev->arg);
      }
    }
  }

  return 0;
}