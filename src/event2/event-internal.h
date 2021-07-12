#ifndef EVENT_INTERNAL
#define EVENT_INTERNAL

#include <sys/queue.h>
#include <time.h>

#include "defer-internal.h"
#include "event_config.h"
#include "evsignal-internal.h"
#include "minheap-internal.h"
#include "mm-internal.h"

#define ev_signal_next ev_.ev_signal.ev_signal_next
#define ev_io_next     ev_.ev_io.ev_io_next
#define ev_io_timeput  ev_.ev_io.ev_io_timeout

#define ev_ncalls  ev_.ev_signal.ev_pcalls
#define ev_pncalls ev_.ev_signal.ev_pncalls

#define ev_pri      ev_evcallback.evcb_flags
#define ev_flags    ev_evcallback.evcb_flags
#define ev_closure  ev_evcallback.evcb_closure
#define ev_callback ev_evcallback.evcb_cb_union.evcb_callback
#define ev_arg      ev_evcallback.evcb_arg

#define EV_CLOSURE_EVENT               0  // 定时信号
#define EV_CLOSURE_EVENT_SIGNAL        1  // 信号
#define EV_CLOSURE_EVENT_PERSIST       2
#define EV_CLOSURE_CB_SELF             3
#define EV_CLOSURE_CB_FINALIZE         4
#define EV_CLOSURE_EVENT_FINALIZE      5
#define EV_CLOSURE_EVENT_FINALIZE_FREE 6

struct eventop {
  const char *name;
  void *(*init)(struct event_base *);  // 初始化
  int (*add)(struct event_base *,
             evutil_socket_t fd,
             short           old,
             short           events,
             void *          fdinfo);  // 注册事件
  int (*del)(struct event_base *,
             evutil_socket_t fd,
             short           old,
             short           events,
             void *          fdinfo);                                // 删除事件
  int (*dispatch)(struct event_base *, struct timeval *);  // 事件分发
  void (*dealloc)(struct event_base *);  // 注销，释放资源

  int                       need_reinit;
  enum event_method_feature features;
  size_t                    fdinfo_len;
};

#define event_io_map event_signal_map

struct event_signal_map {
  void **entries;
  int    nentries;
};

struct common_timeout_list {
  struct event_list  events;
  struct timeval     duration;
  struct event       timeout_event;
  struct event_base *base;
};

#define COMMON_TIMEOUT_MICROSECONDS_MASK 0x000fffff

struct event_change;

struct event_changelist {
  struct event_change *changes;
  int                  n_changes;
  int                  changes_size;
};

#ifdef EVENT__DISABLE_DEBUG_MODE
extern int event_debug_mode_on_;
#define EVENT_DEBUG_MODE_IS_ON() (event_debug_mode_on_)
#else
#define EVENT_DEBUG_MODE_IS_ON() (0)
#endif

TAILQ_HEAD(evcallback_list, event_callback);

struct event_once {
  LIST_ENTRY(event_once) next_once;
  struct event ev;
  void (*cb)(evutil_socket_t, short, void *);
  void *arg;
};

struct event_base {
  const struct eventop *  evsel;  // 可以看做一个class
  void *                  evbase;
  struct event_changelist changelist;
  const struct eventop *  evsigsel;  // 可以看做静态函数
  struct evsig_info       sig;       // 管理信号的结构体
  int                     virtual_event_count;
  int                     virtual_event_count_max;
  int                     event_count;
  int                     event_count_max;
  int                     event_count_active;
  int                     event_count_active_max;
  int                     event_gotterm;
  int                     event_break;
  int                     event_continue;
  int                     event_running_priority;
  int                     running_loop;
  int                     n_defferreds_queued;
  struct evcallback_list
      *                         activequeues;  // 每个一维指针指向一个固定优先级的就绪event序列
  struct evcallback_list        active_later_queue;
  struct common_timeout_list ** common_timeout_queues;
  int                           n_common_timeouts;
  int                           n_common_timeouts_allocated;
  struct event_io_map           io;
  struct event_signal_map       sigmap;
  struct min_heap               timeheap;   // 管理时间的最小堆
  struct timeval                tv_cacehd;  // 时间管理变量
  struct evutil_monotonic_timer monotonic_timer;
  struct timeval                tv_clock_diff;
  time_t                        last_updated_clock_diff;

#ifndef EVENT__DISABLE_THREAD_SUPPORT
  unsigned long th_owner_id;
  void *        th_base_lock;
  void *        current_event_cond;
  int           current_event_waiters;
#endif
  struct event_callback *     current_event;
  enum event_base_config_flag flags;
  struct timeval              max_dispatch_time;
  int                         max_dispatch_callbacks;
  int                         limit_callbacks_after_prio;

  int             is_notify_pending;
  evutil_socket_t th_notify_fd[2];
  struct event    th_notify;
  int (*th_notify_fn)(struct event_base *base);
  struct evutil_weakrand_state weakrand_seed;
  LIST_HEAD(once_event_list, event_once) once_events;
};

struct event_config {
  TAILQ_HEAD(event_configq, event_config_entry) entries;

  int                         n_cpus_hint;
  struct timeval              max_dispatch_interval;
  int                         max_dispatch_callbacks;
  int                         limit_callbacks_after_prio;
  enum event_method_feature   required_features;
  enum event_base_config_flag flags;
};

#ifndef TAILQ_FIRST
#define TAILQ_FIRST(head) ((head)->tqh_first)
#endif

#ifndef TAILQ_END
#define TAILQ_END(head) NULL
#endif

#ifndef TAILQ_NEXT
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)
#endif

#ifndef TAILQ_FOREACH
#define TAILQ_FOREACH(var, head, field)                                        \
  for ((var) = TAILQ_FIRST(head); (var) != TAILQ_END(head);                    \
       (var) = TAILQ_NEXT(var, field))
#endif

#ifndef TAILQ_INSERT_BEFORE
#define TAILQ_INSERT_BEFORE(listelm, elm, field)                               \
  do {                                                                         \
    (elm)->field.tqe_prev      = (listelm)->field.tqe_next;                    \
    (elm)->field.tqe_next      = (listelm);                                    \
    *(listelm)->field.tqe_prev = (elm);                                        \
    (listelm)->field.tqe_prev  = &(elm)->field.tqe_next;                       \
    \                                                                          \
  } while (0)
#endif

#define N_ACTIVE_CALLBACKS(base) ((base)->event_count_active)

int evsig_set_handler_(struct event_base *base, int evsignal, void (*fn)(int));
int evsig_restore_handler_(struct event_base *base, int evsignal);
int evsig_add_noblock_(struct event *        ev,
                       const struct timeval *tv,
                       int                   tv_is_absolute);

#define EVENT_DEL_NOBLOCK             0
#define EVENT_DEL_BLOCK               1
#define EVENT_DEL_AUTOVLOCK           2
#define EVENT_DEL_EVENT_IF_FINALIZING 3

int  event_del_nolock_(struct event *ev, int blocking);
int  event_remove_timer_nolock_(struct event *ev);
void event_active_nolock_(struct event *ev, int res, short count);
int  event_callback_activate_(struct event_base *, struct event_callback *);
int  event_callback_activate_nolock_(struct event_base *,
                                     struct event_callback *);
int  event_callback_cancel_(struct event_base *, struct event_callback *);
void event_callback_finalize_nolock_(struct event_base *    base,
                                     unsigned int           flags,
                                     struct event_callback *evcb,
                                     void (*cb)(struct event_callback *,
                                                void *));
void event_callback_finalize_(struct event_base *    base,
                              unsigned int           flags,
                              struct event_callback *evcb,
                              void (*cb)(struct event_callback *, void *));
int  event_callback_finalize_many_(struct event_base *     base,
                                   int                     n_cbs,
                                   struct event_callback **evcb,
                                   void (*cb)(struct event_callback *, void *));
void event_active_later_(struct event *ev, int res);
void event_active_later_nolock_(struct event *ev, int res);
int  event_callback_activate_later_nolock_(struct event_base *    base,
                                           struct event_callback *evcb);
int  event_callback_cancel_nolock_(struct event_base *    base,
                                   struct event_callback *evcb,
                                   int                    even_if_finalizing);
void event_callback_init_(struct event_base *base, struct event_callback *cb);
void event_base_add_virtual_(struct event_base *base);
void event_base_del_virtual_(struct event_base *base);
void event_base_assert_ok_(struct event_base *base);
void event_base_assert_ok_nolock_(struct event_base *base);
int  event_base_foreach_event_nolock_(struct event_base *         base,
                                      event_base_foreach_event_cb cb,
                                      void *                      arg);
void event_disable_debug_mode(void);
#endif /* EVENT_INTERNAL */
