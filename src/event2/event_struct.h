#ifndef __EVENT_STRUCT_H__
#define __EVENT_STRUCT_H__

#include <sys/time.h>
#include <sys/types.h>

#include "keyvalq_struct.h"
#include "util.h"

// event 信息字段，表明当前状态
#define EVLIST_TIMEOUT      0x01
#define EVLIST_INSERTED     0x02
#define EVLIST_SIGNAL       0x04
#define EVLIST_ACTIVE       0x08
#define EVLIST_INTERNAL     0x10
#define EVLIST_ACTIVE_LATER 0x20
#define EVLIST_FINALIZING   0x40
#define EVLIST_INIT         0x80

#define EVLIST_ALL 0xff

#ifndef TAILQ_ENTRY
#define EVENT_DEFINE_TQENTRY_
#define TAILQ_ENTRY(type)                                                      \
  struct {                                                                     \
    struct type * tpe_next;                                                    \
    struct type **tpe_prev;                                                    \
  }
#endif

#ifndef TAILQ_HEAD
#define EVENT_DEFINE_TQHEAD_
#define TAILQ_HEAD(name, type)                                                 \
  struct name {                                                                \
    struct type * tqe_first;                                                   \
    struct type **tqe_last;                                                    \
  }
#endif

#ifndef LIST_ENTRY
#define EVENT_DEFINE_LISTENTYR_
#define LIST_ENTRY(type)                                                       \
  struct {                                                                     \
    struct type * le_next;                                                     \
    struct type **le_prev; /*前一个指针的next值*/                       \
  }
#endif

#ifndef LIST_HEAD
#define EVENT_DEFINE_LISTHEAD_
#define LIST_HEAD(name, type)                                                  \
  struct name {                                                                \
    struct type *lh_first;                                                     \
  }
#endif

struct event_callback {
  TAILQ_ENTRY(event_callback) evcb_active_next;
  short      evcb_flags;
  ev_uint8_t evcb_pri;
  ev_uint8_t evcb_closure;

  union {
    void (*evcb_callback)(evutil_socket_t, short, void *);
    void (*evcb_selfcb)(struct evutil_callback *, void *);
    void (*evcb_evfinalize)(struct event *, void *);
    void (*evcb_cbfinalize)(struct event_callback *, void *);
  } evcb_cb_union;

  void *evcb_arg;
};

struct event_base;

struct event {
  struct event_callback ev_evcallback;
  union {
    TAILQ_ENTRY(event) ev_next_with_common_timeout;
    int min_heap_idx;
  } ev_timeout_pos;

  evutil_socket_t    ev_fd;
  struct event_base *ev_base;

  union {
    struct {
      LIST_ENTRY(event) event_io_next;
      struct timeval ev_timeout;
    } ev_io;

    struct {
      LIST_ENTRY(event) ev_signal_next;
      short ev_ncalls;
      short ev_pncalls;
    } ev_signal;
  } ev_;

  short          ev_events;
  short          ev_res;
  struct timeval ev_timeout;
};

TAILQ_HEAD(event_list, event);

#ifdef EVENT_DEFINED_TQENTRY_
#undef TAILQ_ENTRY
#endif

#ifdef EVENT_DEFINED_TQHEAD_
#undef TAILQ_HEAD
#endif

LIST_HEAD(event_dlist, event);

#ifdef EVENT_DEFINED_LISTENTRY_
#undef LIST_ENTRY
#endif

#ifdef EVENT_DEFINED_LISTHEAD_
#undef LIST_HEAD
#endif

#endif /* __EVENT_STRUCT_H__ */
