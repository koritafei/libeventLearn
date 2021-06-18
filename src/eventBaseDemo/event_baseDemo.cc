/**
 * @ Description  :
 * @ Version      : 1.0
 * @ Author       : koritafei(koritafei@gmail.com)
 * @ Date         : 2021-06-18 06:00:38
 * @ LastEditors  : koritafei(koritafei@gmail.com)
 * @ LastEditTime : 2021-06-18 08:57:51
 * @ FilePath     : /libevent_l/src/eventBaseDemo/event_baseDemo.cc
 * @ Copyright (C) 2021 koritafei(koritafei@gmail.com). All rights reserved.
 * */

#include <event.h>
#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  struct event_base *base;
  // enum event_method_feature f;
  int f;

  base = event_base_new();
  if (!base) {
    printf("Couldn't get an event_base!\n");
  } else {
    printf("Using Libevent with backend method %s.\n",
           event_base_get_method(base));
    f = event_base_get_features(base);
    if (f & EV_FEATURE_ET) {
      printf(" Edge-triggered events are supported\n");
    }
    if (f & EV_FEATURE_O1) {
      printf(" O(1) event notification is supported\n");
    }

    if (f & EV_FEATURE_FDS) {
      printf(" All FD types are supported\n");
    }

    event_base_free(base);
  }
}
