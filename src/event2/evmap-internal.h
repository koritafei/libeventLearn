#ifndef __EVMAP_INTERNAL_H__
#define __EVMAP_INTERNAL_H__

#include "event2/util.h"

struct event_base;
struct event;

void evmap_io_initmap_(struct event_io_map *ctx);
void evmap_signal_initmap_(struct event_signal_map *ctx);

void evmap_io_clear_(struct event_io_map *ctx);
void evmap_signal_clear_(struct event_signal_map *ctx);

int evmap_io_add_(struct event_base *base,
                  evutil_socket_t    fd,
                  struct event *     ev);
int evmap_io_del_(struct event_base *base,
                  evutil_socket_t    fd,
                  struct event *     ev);

void  evmap_io_active_(struct event_base *base,
                       evutil_socket_t    fd,
                       short              events);
int   evmap_signal_add_(struct event_base *base, int signum, struct event *ev);
int   evmap_signal_del_(struct event_base *base, int signum, struct event *ev);
int   evmap_signal_active_(struct event_base *base,
                           evutil_socket_t    signum,
                           int                ncalls);
void *evmap_io_get_fdinfo_(struct event_io_map *ctx, evutil_socket_t fd);

int  evmap_reinit_(struct event_base *base);
void evmap_delete_all_(struct event_base *base);
void evmap_check_integrity_(struct event_base *base);
int  evmap_foreach_event_(struct event_base *         base,
                          event_base_foreach_event_cb fn,
                          void *                      arg);
#endif /* __EVMAP_INTERNAL_H__ */
