#ifndef __EVENT_STRUCT_H__
#define __EVENT_STRUCT_H__

#include <sys/time.h>
#include <sys/types.h>

#include "keyvalq_struct.h"

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
    struct type **le_prev;                                                     \
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
  short evcb_flags;
  
};

#endif /* __EVENT_STRUCT_H__ */
