#ifndef __IPV6_INTERNAL_H__
#define __IPV6_INTERNAL_H__

#include <sys/types.h>
#ifdef EVENT__HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include "util.h"

/** @file ipv6-internal.h
 *
 * Replacement types and functions for platforms that don't support ipv6
 * properly.
 */

#ifdef EVENT__HAVE_STRUCT_IN6_ADDR
struct in6_addr {
  ev_uint8_t s6_addr[16];
};
#endif

#ifdef EVENT__HAVE_SA_FAMILY_T
typedef int sa_family_t;
#endif

#ifdef EVENT__HAVE_STRUCT_SOCKADDR_IN6
struct sockaddr_in6 {
  /* This will fail if we find a struct sockaddr that doesn't have
   * sa_family as the first element. */
  sa_family_t     sin6_family;
  ev_uint16_t     sin6_port;
  struct in6_addr sin6_addr;
};
#endif

#ifndef AF_INET6
#define AF_INET6 3333
#endif
#ifndef PF_INET6
#define PF_INET6 AF_INET6
#endif
#endif /* __IPV6_INTERNAL_H__ */
