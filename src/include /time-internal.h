#ifndef __TIME_INTERNAL_H__
#define __TIME_INTERNAL_H__

#include <bits/types/struct_timeval.h>
#include <time.h>

#include "util.h"

#if defined(EVENT__HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
#define HAVE_POSIX_MONOTONIC
#elif defined(EVENT__HAVE_MACH_ABSOLUTE_TIME)
#define HAVE_MACH_MONOTONIC
#elif defined(_WIN32)
#define HAVE_WIN32_MONOTONIC
#else
#define HAVE_FALLBACK_MONOTONIC
#endif

long evutil_tv_to_msec_(const struct timeval *tv);
void evutil_usleep_(const struct timeval *tv);

struct evutil_monotonic_timer {
#ifdef HAVE_MATCH_MONOTONIC
  struct match_timebase_info match_timebase_units;
#endif

#ifdef HAVE_POSIX_MONOTONIC
  int monotonic_clock;
#endif

  struct timeval adjust_monotonic_clock;
  struct timeval last_time;
};

int evutil_configure_monotonic_time_(struct evutil_monotonic_timer *timer,
                                     int                            flags);
int evutil_gettime_monotonic_(struct evutil_monotonic_timer *timer,
                              struct timeval *               tp);
#endif /* __TIME_INTERNAL_H__ */
