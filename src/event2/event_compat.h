#ifndef __EVENT_COMPAT_H__
#define __EVENT_COMPAT_H__

#include <event2/util.h>
#include <sys/time.h>
#include <sys/types.h>

struct event_base *event_init(void);
int                event_dispatch(void);
int                event_loop(int);
int                event_loopexit(const struct timeval *tv);
int                event_loopbreak(void);

int event_once(evutil_socket_t,
               short,
               void (*)(evutil_socket_t, short, void *),
               void *,
               const struct timeval *);

const char *event_get_method(void);
int         event_priority_init(int);

void event_set(struct event *,
               evutil_socket_t,
               short,
               void (*)(evutil_socket_t, short, void *),
               void *);

#define ev_timerset(ev, cb, arg) event_set((ev), -1, 0, (cb), (arg))
#define ev_sinalset(ev, x, cb, arg)                                            \
  event_set((ev), x, EV_SIGNAL | EV_PERSIST, (cb), (arg))

#define timeout_add(ev, tv)      event_add((ev), (tv))
#define timeout_set(ev, cb, arg) event_set((ev), -1, 0, (cb), (arg))
#define timeout_del(ev)          event_del((ev))
#define timeout_pending(ev, tv)  event_pending((ev), (tv))
#define timeout_initialized(ev)  event_initialized((ev))

#define signal_add(ev, tv) event_add((ev), (tv))
#define signal_set(ev, x, cb, arg)                                             \
  event_set((ev), (x), EV_SIGNAL | EV_PERSIST, (cb), (arg))
#define signal_del(ev)         event_del((ev))
#define signal_pending(ev, tv) event_pending((ev), EV_SIGNAL, (tv))
#define signal_initialized(ev) event_initialized((ev))

#ifndef EVENT_FD
#define EVENT_FD(ev)     ((int)event_get_fd((ev)))
#define EVENT_SIGNAL(ev) event_get_signal((ev))
#endif

#endif /* __EVENT_COMPAT_H__ */
