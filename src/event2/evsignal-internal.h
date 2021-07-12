#ifndef __EV_SIGNAL_INTERNAL_H__
#define __EV_SIGNAL_INTERNAL_H__

#include <event2/util.h>
#include <signal.h>

typedef void (*ev_signalhander_t)(int);

struct evsig_info {
  struct event    ev_signal;          // 事件监控为ev_signal_pair[1]
  evutil_socket_t ev_signal_pair[2];  // 用于发送事件通知
  int ev_signal_added;  // 设置signal被添加的标志(true，一已添加)
  int ev_n_signals_added;  // 已添加signal的计数

#ifdef EVENT__HAVE_SIGNAL
  struct sigaction **sh_old;
#else
  ev_sigev_signalhander_t **sh_old;
#endif
  int sh_old_max;  // sh_old 最大值
};

int  evsig_init_(struct event_base *);
void evsig_dealloc_(struct event_base *);
void evsig_set_base_(struct event_base *base);
void evsig_free_globals_();
#endif /* __EV_SIGNAL_INTTERNAL_H__ */
