/**
 * @Copyright (c) 2021  koritafei
 * @file buffereventwatermark.cc
 * @brief
 * @author koritafei (koritafei@gmail.com)
 * @version 0.1
 * @date 2021-06-22 02:06:17
 *
 * */

#include <ctype.h>
#include <errno.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct info {
  const char *name;
  size_t      total_drained;
};

void read_callback(struct bufferevent *bev, void *ctx) {
  struct info *    inf    = (struct info *)ctx;
  struct evbuffer *input  = bufferevent_get_input(bev);
  size_t           length = evbuffer_get_length(input);

  if (length) {
    inf->total_drained += length;
    evbuffer_drain(input, length);
    printf("Drained %lu bytes from %s\n", (unsigned long)length, inf->name);
  }
}

void event_callback(struct bufferevent *bev, short events, void *ctx) {
  struct info *    inf      = (struct info *)ctx;
  struct evbuffer *input    = bufferevent_get_input(bev);
  int              finished = 0;

  if (events & BEV_EVENT_EOF) {
    size_t length = evbuffer_get_length(input);
    printf(
        "Got a close from %s. We drained %lu bytes from it, and had %lu bytes "
        "left",
        inf->name,
        (unsigned long)inf->total_drained,
        (unsigned long)length);
    finished = 1;
  }

  if (events & BEV_EVENT_ERROR) {
    printf("Got an error from %s:%s\n",
           inf->name,
           evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    finished = 1;
  }

  if (finished) {
    free(ctx);
    bufferevent_free(bev);
  }
}

struct bufferevent *setup_bufferevent() {
  struct bufferevent *b1    = NULL;
  struct info *       info1 = (struct info *)malloc(sizeof(struct info));
  info1->name               = "buffer 1";
  info1->total_drained      = 0;

  bufferevent_setwatermark(b1, EV_READ, 128, 0);

  bufferevent_setcb(b1, read_callback, NULL, event_callback, info1);

  bufferevent_enable(b1, EV_READ);

  return b1;
}

void read_callback_uppercase(struct bufferevent *bev, void *ctx) {
  char   tmp[128];
  size_t n;
  int    i;
  while (1) {
    n = bufferevent_read(bev, tmp, sizeof(tmp));
    if (0 >= n) {
      break;
    }

    for (i = 0; i <= n; i++) {
      tmp[i] = toupper(tmp[i]);
    }

    bufferevent_write(bev, tmp, n);
  }
}

struct proxy_info {
  struct bufferevent *other_bev;
};

void read_callback_proxy(struct bufferevent *bev, void *ctx) {
  struct proxy_info *info = (struct proxy_info *)bev;
  bufferevent_read_buffer(bev, bufferevent_get_output(info->other_bev));
}

struct count {
  unsigned long last_fib[2];
};

void write_callback_fibonacci(struct bufferevent *bev, void *ctx) {
  struct count *   c  = (struct count *)ctx;
  struct evbuffer *ev = evbuffer_new();
  while (1024 > evbuffer_get_length(ev)) {
    unsigned long next = c->last_fib[0] + c->last_fib[1];
    c->last_fib[0]     = c->last_fib[1];
    c->last_fib[1]     = next;

    evbuffer_add_printf(ev, "%lu", next);
  }

  bufferevent_write_buffer(bev, ev);
  evbuffer_free(ev);
}

int main(int argc, char **argv) {
}
