#ifndef __KEYVALQ_STRUCT_H__
#define __KEYVALQ_STRUCT_H__

#ifndef TAILQ_ENTRY
#define EVENT_DEFINE_TQENTRY_
#define TAILQ_ENTRY(type)                                                        \
  struct {                                                                       \
    struct type*  tqe_next; /* 指向下一地址 */                             \
    struct type** tqe_prev; /* 前驱，指向下一个地址的前驱指针  */ \
  }
#endif /* TAILQ_ENTRY */

#ifndef TAILQ_HEAD
#define EVENT_DEFINE_TQHEAD_
#define TAILQ_HEAD(name, type)                                                 \
  struct name {                                                                \
    struct type*  tqh_first;                                                   \
    struct type** tqh_last;                                                    \
  }
#endif /* TAILQ_HEAD */

/**
 * @brief key-value struct
 *
 */
struct evkeyval {
  TAILQ_ENTRY(evkeyval) next;
  char* key;
  char* val;
};

TAILQ_HEAD(evkeyvalq, evkeyval);

#ifdef EVENT_DEFINE_TQENTRY_
#undef EVENT_DEFINE_TQENTRY
#endif

#ifdef EVENT_DEFINE_TQHEAD_
#undef EVENT_DEFINE_TQHEAD
#endif

#endif /* __KEYVALQ_STRUCT_H__ */
