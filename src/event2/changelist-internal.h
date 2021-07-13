#ifndef __CHANGELIST_INTERNAL_H__
#define __CHANGELIST_INTERNAL_H__

#include "util.h"

struct event_change {
  evutil_socket_t fd;
  short           old_events;
  ev_uint8_t      read_change;
  ev_uint8_t      write_change;
  ev_uint8_t      close_change;
};

#define EV_CHANGE_ADD             0x01
#define EV_CHANGE_DEL             0x02
#define EV_CHANGE_SIGNAL          EV_SIGNAL
#define EV_CHANGE_PERSIST         EV_PERSIST
#define EV_CHANGE_ET              EV_ET
#define EV_CHANGELIST_FDINFO_SIZE sizeof(int)

void event_changelist_init(struct event_change *changelist);
void event_changelist_remove_all_(struct event_changelist *changelist,
                                  struct event_base *      base);
void event_changelist_freemem_(struct event_changelist *changelist);
void event_changelist_add_(struct event_base *base,
                           int                fd,
                           short              old,
                           short              events,
                           void *             p);
void event_changelist_del_(struct event_base *base,
                           int                fd,
                           short              old,
                           short              events,
                           void *             p);
#endif /* __CHANGELIST_INTERNAL_H__ */
