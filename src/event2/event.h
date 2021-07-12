#ifndef __EVENT_H__
#define __EVENT_H__

#include <bits/types/struct_timeval.h>
#include <event2/util.h>
#include <stdio.h>

#include "event_config.h"

struct event;
struct event_config;

void event_enable_debug_mode(void);
void event_debug_unassign(struct event *);

struct event_base *event_base_new();
int                event_reinit(struct event_base *);
int                event_base_dispatch(struct event_base *);
const char *       event_base_get_method(const struct event_base *);
const char **      event_get_supported_methods(void);
int event_gettime_monotonic(struct event_base *base, struct timeval *tp);

#define EVENT_BASE_COUNT_ACTIVE  1U
#define EVENT_BASE_COUNT_VIRTUAL 2U
#define EVENT_BASE_COUNT_ADDED   4U

int event_base_get_num_events(struct event_base *, unsigned int);
int event_base_get_max_events(struct event_base *, unsigned int, int);
struct event_config *event_config_new(void);
void                 event_config_free(struct event_config *cfg);
int event_config_avoid_method(struct event_config *cfg, const char *method);

enum event_method_feature {
  EV_FEATURE_ET          = 0x01,
  EV_FEATURE_O1          = 0x02,
  EV_FEATURE_FDS         = 0x04,
  EV_FEATURE_EARLY_CLOSE = 0x08
};

enum event_base_config_flag {
  EVENT_BASE_FLAG_NOLOCK               = 0x01,
  EVENT_BASE_FLAG_IGNORE_ENV           = 0x02,
  EVENT_BASE_FLAG_STARTUP_IOCP         = 0x04,
  EVENT_BASE_FLAG_NO_CACHE_TIME        = 0x08,
  EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10,
  EVENT_BASE_FLAG_PRECISE_TIMER        = 0x20
};

int event_base_get_features(const struct event_base *base);
int event_config_require_features(struct event_config *cfg, int feature);
int event_config_set_flag(struct event_config *cfg, int flag);
int event_config_set_num_cpus_hint(struct event_config *cfg, int cpus);
int event_config_set_max_dispatch_interval(struct event_config * cfg,
                                           const struct timeval *max_interval,
                                           int                   max_callbacks,
                                           int                   min_priority);
struct event_base *event_new_with_config(const struct event_config *);
void               event_base_free(struct event_base *);
void               event_base_free_nofinalize(struct event_base *);

#define EVENT_LOG_DEBUG 0
#define EVENT_LOG_MSG   1
#define EVENT_LOG_WRAN  2
#define EVENT_LOG_ERR   3

#define _EVENT_LOG_DEBUG EVENT_LOG_DEBUG
#define _EVENT_LOG_MSG   EVENT_LOG_MSG
#define _EVENT_LOG_WRAN  EVENT_LOG_WRAN
#define _EVENT_LOG_ERR   EVENT_LOG_ERR

typedef void (*event_log_cb)(int sererity, const char *msg);
void event_set_log_callback(event_log_cb cb);
typedef void (*event_fatal_cb)(int err);
void event_set_fatal_callback(event_fatal_cb cb);

#define EVENT_DBG_ALL  0xffffffffu
#define EVENT_DBG_NONE 0

void event_enable_debug_logging(ev_uint32_t which);
int  event_base_set(struct event_base *, struct event *);

#define EVLOOP_ONCE             0x01
#define EVLOOP_NONBLOCK         0x02
#define EVLOOP_NO_EXIT_ON_EMPTY 0x04

int event_base_loop(struct event_base *, int);
int event_base_loopexit(struct event_base *, const struct timeval *);
int event_base_loopbreak(struct event_base *);
int event_base_loopcontinue(struct event_base *);
int event_base_got_exit(struct event_base *);
int event_base_got_break(struct event_base *);

#define EV_TIMEOUT  0x01
#define EV_READ     0x02
#define EV_WRITE    0x04
#define EV_SIGNAL   0x08
#define EV_PERSIST  0x10
#define EV_ET       0x20
#define EV_FINALIZE 0x40
#define EV_CLOSED   0x80

// timer function
#define evtimer_assign(ev, b, cb, arg)                                         \
  event_assign((ev), (b), -1, 0, (cb), (arg))
#define evtimer_new(b, cb, arg) event_new((b), -1, 0, (cb), (arg))
#define evtimer_add(ev, tv)     event_add((ev), (tv))
#define evtimer_del(ev)         event_del((ev))
#define evtimer_pending(ev, tv) event_pending((ev), EV_TIMEOUT, (tv))
#define evtimer_initialized(ev) event_initialized((ev))

// signal function
#define evsignal_add(ev, tv) event_add((ev), (tv))
#define evsignal_assign(ev, b, x, cb, arg)                                     \
  event_assign((ev), (b), -1, 0, (cb), (arg))
#define evsignal_new(b, x, cb, arg)                                            \
  event_new((b), (x), EV_SIGNAL | EV_PERSIST, (cb), (arg))
#define evsignal_del(ev)         event_del((ev))
#define evsignal_pending(ev, tv) event_pending((ev), EV_SIGNAL, (tv))
#define evsignal_initialized     event_initialized((ev))

// user function
#define evuser_new(b, cb, arg) event_new((b), -1, 0, (cb), (arg))
#define evuser_del(ev)         event_delete((ev))
#define evuser_pending(ev, tv) event_pending((ev), 0, (tv))
#define evuser_intialized(ev)  event_initialized((ev))
#define evuser_trigger(ev)     event_active((ev), 0, 0)

typedef void (*event_callback_fn)(evutil_socket_t, short, void *);

void *event_self_cbarg();

struct event *event_new(struct event_base *,
                        evutil_socket_t,
                        short,
                        event_callback_fn,
                        void *);

int event_assign(struct event *,
                 struct event_base *,
                 int,
                 short,
                 event_callback_fn,
                 void *);

void event_free(struct event *);

typedef void (*event_finalize_callback_fn)(struct event *, void *);
int  event_finalize(unsigned int, struct event *, event_finalize_callback_fn);
int  event_free_finalize(unsigned int,
                         struct event *,
                         event_finalize_callback_fn);
int  event_base_once(struct event_base *,
                     evutil_socket_t,
                     short,
                     event_callback_fn,
                     void *,
                     const struct timerval *);
int  event_add(struct event *ev, const struct timerval *tv);
int  event_remove_timer(struct event *ev);
int  event_del(struct event *);
int  event_del_block(struct event *ev);
void event_active(struct event *ev, int res, short ncalls);
int  event_pending(const struct event *ev, short events, struct timeval *tv);
struct event *    event_base_get_event(struct event_base *);
int               event_initialized(const struct event *ev);
evutil_socket_t   event_get_fd(const struct event *ev);
struct event_base event_get_base(const struct event *ev);
short             event_get_events(const struct event *ev);
event_callback_fn event_get_callback(const struct event *ev);
void *            event_get_callback_arg(const struct event *ev);
int               event_get_priority(const struct event *ev);
void              event_get_assignment(const struct event *event,
                                       struct event_base **base_out,
                                       int *               fd_out,
                                       short *             events_out,
                                       event_callback_fn * callback_out,
                                       void **             arg_out);
size_t            event_get_struct_event_size();
const char *      event_get_version();
ev_uint32_t       event_get_version_number();

#define LIBEVENT_VERSION        EVENT__VERSION
#define LIBEVENT_VERSION_NUMBER EVENT__NUMERIC_VERSION
#define EVENT_MAX_PRIORITES     256

int                    event_base_priority_init(struct event_base *, int);
int                    event_base_get_npriorities(struct event_base *eb);
int                    event_priority_set(struct event *, int);
const struct timerval *event_base_init_common_timeout(
    struct event_base *    base,
    const struct timerval *duration);

#if !defined(EVENT__DISABLE_MM_REPLACEMENT) || defined(EVENT_IN_DOXYGEN_)
void event_set_mem_functions(void *(*malloc_fn)(size_t),
                             void *(*realloc_fn)(void *, size_t),
                             void (*free_fn)(void *));

#define EVENT_SET_MEM_FUNCTIONS_IMPLEMENTED
#endif

void event_base_dump_events(struct event_base *, FILE *);
void event_base_active_by_fd(struct event_base *base, int fd, short events);
void event_base_active_by_signal(struct event_base *base, int sig);

typedef int (*event_base_foreach_event_cb)(const struct event_base *,
                                           const struct event *,
                                           void *);
int event_base_foreach_event(struct event_base *         base,
                             event_base_foreach_event_cb fn,
                             void *                      arg);
int event_base_gettimeofday_cached(struct event_base *base, struct timeval *tv);
int event_base_update_cache_time(struct event_base *base);
void libevent_global_shutdown();
#endif /* __EVENT_H__ */
