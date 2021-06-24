/**
 * @Copyright (c) 2021  koritafei
 * @file buffereventdemo.cc
 * @brief
 * @author koritafei (koritafei@gmail.com)
 * @version 0.1
 * @date 2021-06-22 02:06:02
 *
 * */

#include <event2/bufferevent.h>
#include <event2/event.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void eventcb(struct bufferevent* bev, short event, void* ptr) {
  if (event & BEV_EVENT_CONNECTED) {
    printf("connected \n");
  } else if (event & BEV_EVENT_ERROR) {
    printf("errored\n");
  }
}

int main_loop() {
  struct event_base*  base;
  struct bufferevent* bev;
  struct sockaddr_in  sin;

  base = event_base_new();
  memset(&sin, 0, sizeof(sin));
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port        = htons(8080);

  bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, NULL, NULL, eventcb, NULL);

  if (0 > bufferevent_socket_connect(bev,
                                     (struct sockaddr*)&sin,
                                     sizeof(struct sockaddr))) {
    bufferevent_free(bev);
    return -1;
  }

  return 0;
}

int main(int argc, char** argv) {
  main_loop();
}
