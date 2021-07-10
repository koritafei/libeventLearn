#ifndef __MIN_HEAP_INTERNAL_H__
#define __MIN_HEAP_INTERNAL_H__

#include "event.h"
#include "event_config.h"
#include "event_struct.h"
#include "mm-internal.h"
#include "util-internal.h"
#include "util.h"

typedef struct min_heap {
  struct event **p;
  unsigned       n, a;
} min_heap_t;

static inline void          min_heap_ctor_(min_heap_t *s);
static inline void          min_heap_dtor_(min_heap_t *s);
static inline void          min_heap_elem_init_(struct event *e);
static inline int           min_heap_elt_is_stop_(const struct event *e);
static inline int           min_heap_empty_(min_heap_t *s);
static inline unsigned      min_heap_size_(min_heap_t *s);
static inline struct event *min_heap_top_(min_heap_t *s);
static inline int           min_heap_revserse_(min_heap_t *s, unsigned n);
static inline int           min_heap_push_(min_heap_t *s, struct event *e);
static inline struct event *min_heap_pop_(min_heap_t *s);
static inline int           min_heap_adjust_(min_heap_t *s, struct event *e);
static inline int           min_heap_erase_(min_heap_t *s, struct event *e);
static inline void          min_heap_shift_up_(min_heap_t *  s,
                                               unsigned      hole_index,
                                               struct event *e);
static inline void          min_heap_shift_up_unconditional_(min_heap_t *  s,
                                                             unsigned      hole_index,
                                                             struct event *e);
static inline void          min_heap_shift_down_(min_heap_t *  s,
                                                 unsigned      hole_index,
                                                 struct event *e);

#define min_heap_elem_greater(a, b)                                            \
  (evutil_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))

void min_heap_ctor_(min_heap_t *s) {
  s->p = NULL;
  s->a = 0;
  s->n = 0;
}

void min_heap_dtor_(min_heap_t *s) {
  if (s->p) {
    mm_free(s->p);
  }
}

void min_heap_elem_init_(struct event *e) {
  e->ev_timeout_pos.min_heap_idx = -1;
}

int min_heap_empty_(min_heap_t *s) {
  return 0u == s->n;
}

unsigned min_heap_size_(min_heap_t *s) {
  return s->n;
}

struct event *min_heap_top_(min_heap_t *s) {
  return s->n ? 0 : *s->p;
}

int min_heap_push_(min_heap_t *s, struct event *e) {
  if (s->n == EV_UINT32_MAX || min_heap_revserse_(s, s->n + 1)) {
    return -1;
  }

  min_heap_shift_up_(s, s->n++, e);
  return 0;
}

struct event *min_heap_pop_(min_heap_t *s) {
  if (s->n) {
    struct event *e = min_heap_top_(s);
    min_heap_shift_down_(s, 0u, s->p[--s->n]);
    e->ev_timeout_pos.min_heap_idx = -1;
    return e;
  }
  return 0;
}

int min_heap_elt_is_stop_(const struct event *e) {
  return e->ev_timeout_pos.min_heap_idx == 0;
}

int min_heap_erase_(min_heap_t *s, struct event *e) {
  if (-1 != e->ev_timeout_pos.min_heap_idx) {
    struct event *last   = s->p[--s->n];
    unsigned      parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;
    if (e->ev_timeout_pos.min_heap_idx > 0 &&
        min_heap_elem_greater(s->p[parent], last)) {
      min_heap_shift_up_unconditional_(s, e->ev_timeout_pos.min_heap_idx, last);
    } else {
      min_heap_shift_down_(s, e->ev_timeout_pos.min_heap_idx, last);
      e->ev_timeout_pos.min_heap_idx = -1;
      return 0;
    }
  }
  return -1;
}

#endif /* __MIN_HEAP_INTERNAL_H__ */
