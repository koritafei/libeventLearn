/**
 * @Copyright (c) 2021  koritafei
 * @file eventnew.cc
 * @brief
 * @author koritafei (koritafei@gmail.com)
 * @version 0.1
 * @date 2021-06-21 03:06:55
 *
 * */

#include <event2/event.h>

void cb(evutil_socket_t fd, short what, void *arg) {
  const char *data = (const char *)arg;
  printf("Got an event on socket %d:%s%s%s%s [%s]\n",
         (int)fd,
         (what & EV_TIMEOUT) ? "timeout" : "",
         (what & EV_READ) ? "read" : "",
         (what & EV_WRITE) ? "write" : "",
         (what & EV_SIGNAL) ? "signal" : "",
         data);
}

void main_loop(evutil_socket_t fd1, evutil_socket_t fd2) {
  struct event *     ev1, *ev2;
  struct timeval     five_second = {5, 0};
  struct event_base *base        = event_base_new();

  ev1 = event_new(base,
                  fd1,
                  EV_TIMEOUT | EV_READ | EV_PERSIST,
                  cb,
                  (char *)("Read Event"));
  ev2 =
      event_new(base, fd2, EV_WRITE | EV_PERSIST, cb, (char *)("Write Event"));

  event_add(ev1, &five_second);
  event_add(ev2, NULL);
  event_base_dispatch(base);
}

int main(int argc, char **argv) {
}