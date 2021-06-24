/**
 * @Copyright (c) 2021  koritafei
 * @file eventLoopDemo.cc
 * @brief
 * @author koritafei (koritafei@gmail.com)
 * @version 0.1
 * @date 2021-06-20 08:06:83
 *
 * */

#include <event2/event.h>
#include <event2/util.h>

#include <iostream>

void cb(int sock, short what, void *arg) {
  struct event_base *base = (struct event_base *)arg;
  event_base_loopbreak(base);
}

void main_loop(struct event_base *base, evutil_socket_t watchdag_fd) {
  struct event *watchdag_event;
  watchdag_event = event_new(base, watchdag_fd, EV_READ, cb, base);

  event_add(watchdag_event, NULL);
  event_base_dispatch(base);
}

void run_base_with_ticks(struct event_base *base) {
  struct timeval ten_sec;
  ten_sec.tv_sec  = 10;
  ten_sec.tv_usec = 0;

  while (1) {
    event_base_loopexit(base, &ten_sec);
    event_base_dispatch(base);

    puts("Ticks");
  }
}

int main(int argc, char **argv) {
  struct event_base *base = event_base_new();
  run_base_with_ticks(base);
}
