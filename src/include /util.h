#ifndef __UTIL_H__
#define __UTIL_H__

#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#define ev_uint64_t uint64_t
#define ev_int64_t  int64_t

#define ev_uint32_t uint32_t
#define ev_int32_t  int32_t

#define ev_uint16_t uint16_t
#define ev_int16_t  int16_t

#define ev_uint8_t uint8_t
#define ev_int8_t  int8_t

#define ev_uintptr_t uintptr_t
#define ev_intptr_t  intptr_t

#define ev_ssize_t ssize_t

#define EV_UINT64_MAX UINT64_MAX
#define EV_INT64_MAX  INT64_MAX
#define EV_UINT32_MAX UINT32_MAX
#define EV_INT32_MAX  INT32_MAX
#define EV_UINT16_MAX UINT16_MAX
#define EV_INT16_MAX  INT16_MAX
#define EV_UINT8_MAX  UINT8_MAX
#define EV_INT8_MAX   INT8_MAX
#define EV_INT64_MIN  INT64_MIN
#define EV_INT32_MIN  INT32_MIN
#define EV_INT16_MIN  INT16_MIN
#define EV_INT8_MIN   INT8_MIN

#endif /* __UTIL_H__ */
