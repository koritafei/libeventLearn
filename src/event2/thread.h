#ifndef __THREAD_H__
#define __THREAD_H__

#include "event_config.h"

#define EVTHRED_WRITE 0x04
#define EVTHRED_READ  0x08
#define EVTHRED_TRY   0x10

#if !defined(EVENT__DISABLE_THREAD_SUPPORT) || defined(EVENT_IN_DOXYGEN_)

#define EVTHRED_LOCK_API_VERSION    1
#define EVTHREAD_LOCKTYPE_RECURSE   1
#define EVTHREAD_LOCKTYPE_READWRITE 2

struct evthread_lock_callbacks {
  int      lock_api_version;
  unsigned supported_locktypes;

  void *(*alloc)(unsigned locktype);
  void (*free)(void *lock, unsigned locktype);
  void (*lock)(unsigned mode, void *lock);
  void (*unlock)(unsigned mode, void *lock);
};

int evthread_set_lock_callbacks(const struct evthread_lock_callbacks *);

#define EVTHRED_CONDITION_API_VERSION 1

struct timeval;

struct evthread_condition_callbacks {
  int condition_api_version;
  void *(*alloc_condition)(unsigned condtype);
  void (*free_condition)(void *cond);
  int (*signal_condition)(void *cond, int boardcast);

  int (*wait_condition)(void *cond, void *lock, const struct timeval *tv);
};

int evthread_set_condition_callbacks(
    const struct evthread_condition_callbacks *);
void evthread_set_id_callbacks(unsigned long (*id_fn)());

#if defined(EVENT__HAVE_PTHREADS) || defined(EVENT_IN_DOXYGEN_)
int evthread_use_pthreads();
#define EVTHREAD_USE_PTHREADS_IMPLEMENTED 1
#endif

void evthread_enable_lock_debuging();
void evthread_enable_lock_debugging();
#endif

struct event_base;
int evthread_make_base_notifiable(struct event_base *base);

#endif /* __THREAD_H__ */
