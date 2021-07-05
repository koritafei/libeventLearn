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

// 类型定义
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

// 最值定义
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

#define EV_SIZE_MAX  EV_UINT64_MAX
#define EV_SSIZE_MAX EV_INT64_MAX

#define EV_SSIZE_MIN    (-(EV_SSIZE_MAX)-1)
#define ev_socket_t     socklen_t
#define evutil_socket_t int

/**
 * @brief monotonic timer(单调定时器) struct
 *        单调定时器结构
 */
struct evutil_monotonic_timer;

#define EV_MONOT_PRECISE  1
#define EV_MONOT_FALLBACK 2

/**
 * @brief format a date string using RFC1123 format(used in http)
 *        以RFC1123格式(http使用)格式化日期字符串
 * @param date    日期字符串
 * @param datelen 日期字符串长度
 * @param tm      如果为NULL，则使用系统时间
 * @return int    返回值，返回转化后字符串长度，需与datelen比较，避免截断
 */
int evutil_date_rfc1123(char *date, const size_t datelen, const struct tm *tm);

/**
 * @brief 创建一个新的 evutil_mononic_timer结构体
 *
 * @return struct evutil_monotonic_timer*
 */
struct evutil_monotonic_timer *evutil_monotonic_timer_new();

/**
 * @brief 释放一个evutil_monotoic_timer结构体
 *
 * @param timer
 */
void evutil_monotonic_timer_free(struct evutil_monotonic_timer *timer);
/**
 * @brief 配置一个evutil_monotonic_timer 结构体
 *
 * @param timer
 * @param flags 可以包含EV_MONOT_PRECIST和EV_MONOT_FALLBACK
 * @return int
 */
int evutil_configure_monotonic_time(struct evutil_monotonic_timer *timer,
                                    int                            flags);
/**
 * @brief 非线程安全，获取monotonic timer
 *
 * @param timer
 * @param tp
 * @return int
 */
int evutil_gettime_monotonic(struct evutil_monotonic_timer *timer,
                             struct timeval *               tp);

/**
 * @brief
 *
 * @param d
 * @param type
 * @param protocol
 * @param sv
 * @return int
 */
int evutil_socketpair(int d, int type, int protocol, int *sv);
int evutil_make_socket_nonblocking(evutil_socket_t fd);
int evutil_make_listen_socket_reuseable(evutil_socket_t sock);
int evutil_make_listen_socket_reuseable_port(evutil_socket_t sock);
int evutil_make_listen_socket_ipv6only(evutil_socket_t sock);
int evutil_make_socket_closeonexec(evutil_socket_t sock);
int evutil_closesocket(evutil_socket_t sock);
#define EV_CLOSESOCKET(s) evutil_closesocket(s)

int evutil_make_tcp_listen_socket_defferred(evutil_socket_t sock);

#define EVUTIL_SOCKET_ERROR() (errno)
#define EVUTIL_SET_SOCKET_ERROR(errcode)                                       \
  do {                                                                         \
    errno = errcode;                                                           \
  } while (0)

#define evutil_socket_geterror(sock)        (errno)
#define evutil_socket_error_to_string(sock) (strerror(errno))
#define EVUTIL_INVALID_SOCKET               -1

#define evutil_timeradd(tvp, uvp, vvp)                                         \
  do {                                                                         \
    (vvp)->tv_sec  = (tvp)->tv_sec + (uvp)->tv_sec;                            \
    (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;                          \
    if ((vvp)->tv_usec >= 1000000) {                                           \
      (vvp)->tv_sec++;                                                         \
      (vvp)->tv_usec -= 1000000;                                               \
    }                                                                          \
  } while (0)
#define evutil_timersub(tvp, uvp, vvp)                                         \
  do {                                                                         \
    (vvp)->tv_sec  = (tvp)->tv_sec - (uvp)->tv_sec;                            \
    (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;                          \
    if ((vvp)->tv_usec < 0) {                                                  \
      (vvp)->tv_sec--;                                                         \
      (vvp)->tv_usec -= 1000000;                                               \
    }                                                                          \
  } while (0)
#define evutil_timeclear(tvp) (tvp)->tv_sec = (tvp)->tv_usec = 0
#define evutil_timercmp(tvp, uvp, cmp)                                         \
  (((tvp)->tv_sec == (uvp)->tv_sec ? ((tvp)->tv_usec cmp(uvp)->tv_usec)        \
                                   : ((tvp)->tv_sec cmp(uvp)->tv_sec)))
#define evutil_timeriset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)

#define offset(type, field) ((off_t)(&(type *)0->field))

ev_int64_t evutil_strtoll(const char *s, char **endptr, int base);

struct timezone;
int evutil_gettimeofday(struct timeval *tv, struct timezone *tz);
int evutil_snprintf(char *buf, size_t buflen, const char *fmt, ...);
int evutil_vsnprintf(char *buf, size_t buflen, const char *fmt, va_list ap);
const char *evutil_inet_ntoa(int af, const void *src, char *dst, size_t len);
int         evutil_inet_pton_scope(int         af,
                                   const void *src,
                                   char *      dst,
                                   unsigned *  indexp);
int         evutil_inet_pton(int af, const void *src, void *dst);

struct socketaddr;
int evutil_parse_socket_port(const char *       str,
                             struct socketaddr *out,
                             int *              outlen);

int evutil_socketaddr_cmp(const struct socketaddr *sa1,
                          const struct socketaddr *sa2,
                          int                      include_port);
int evutil_assii_strcasecmp(const char *str1, const char *str2);
int evutil_assii_strncmp(const char *str1, const char *str2, size_t n);

struct evutil_addrinfo {
  int                     ai_flags;
  int                     ai_family;
  int                     ai_sockettype;
  int                     ai_protocol;
  size_t                  ai_socklen;
  char *                  cannonname;
  struct sockaddr *       ai_addr;
  struct evutil_addrinfo *ai_next;
};

#if defined(EAI_ADDRFAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_ADDRFAMILY EAI_ADDRFAMILY
#else
#define EVUTIL_EAI_ADDRFAMILY -901
#endif
#if defined(EAI_AGAIN) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_AGAIN EAI_AGAIN
#else
#define EVUTIL_EAI_AGAIN -902
#endif
#if defined(EAI_BADFLAGS) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_BADFLAGS EAI_BADFLAGS
#else
#define EVUTIL_EAI_BADFLAGS -903
#endif
#if defined(EAI_FAIL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAIL EAI_FAIL
#else
#define EVUTIL_EAI_FAIL -904
#endif
#if defined(EAI_FAMILY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_FAMILY EAI_FAMILY
#else
#define EVUTIL_EAI_FAMILY -905
#endif
#if defined(EAI_MEMORY) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_MEMORY EAI_MEMORY
#else
#define EVUTIL_EAI_MEMORY -906
#endif
/* This test is a bit complicated, since some MS SDKs decide to
 * remove NODATA or redefine it to be the same as NONAME, in a
 * fun interpretation of RFC 2553 and RFC 3493. */
#if defined(EAI_NODATA) && defined(EVENT__HAVE_GETADDRINFO) &&                 \
    (!defined(EAI_NONAME) || EAI_NODATA != EAI_NONAME)
#define EVUTIL_EAI_NODATA EAI_NODATA
#else
#define EVUTIL_EAI_NODATA -907
#endif
#if defined(EAI_NONAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_NONAME EAI_NONAME
#else
#define EVUTIL_EAI_NONAME -908
#endif
#if defined(EAI_SERVICE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SERVICE EAI_SERVICE
#else
#define EVUTIL_EAI_SERVICE -909
#endif
#if defined(EAI_SOCKTYPE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SOCKTYPE EAI_SOCKTYPE
#else
#define EVUTIL_EAI_SOCKTYPE -910
#endif
#if defined(EAI_SYSTEM) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_EAI_SYSTEM EAI_SYSTEM
#else
#define EVUTIL_EAI_SYSTEM -911
#endif

#define EVUTIL_EAI_CANCEL -90001

#if defined(AI_PASSIVE) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_PASSIVE AI_PASSIVE
#else
#define EVUTIL_AI_PASSIVE 0x1000
#endif
#if defined(AI_CANONNAME) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_CANONNAME AI_CANONNAME
#else
#define EVUTIL_AI_CANONNAME 0x2000
#endif
#if defined(AI_NUMERICHOST) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICHOST AI_NUMERICHOST
#else
#define EVUTIL_AI_NUMERICHOST 0x4000
#endif
#if defined(AI_NUMERICSERV) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_NUMERICSERV AI_NUMERICSERV
#else
#define EVUTIL_AI_NUMERICSERV 0x8000
#endif
#if defined(AI_V4MAPPED) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_V4MAPPED AI_V4MAPPED
#else
#define EVUTIL_AI_V4MAPPED 0x10000
#endif
#if defined(AI_ALL) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ALL AI_ALL
#else
#define EVUTIL_AI_ALL 0x20000
#endif
#if defined(AI_ADDRCONFIG) && defined(EVENT__HAVE_GETADDRINFO)
#define EVUTIL_AI_ADDRCONFIG AI_ADDRCONFIG
#else
#define EVUTIL_AI_ADDRCONFIG 0x40000
#endif

struct evutil_addrinfo;

int         evutil_getaddrinfo(const char *                  nodename,
                               const char *                  servname,
                               const struct evutil_addrinfo *hints_in,
                               struct evutil_addrinfo **     res);
void        evutil_freeaddrinfo(struct evutil_addrinfo *ai);
const char *evutil_gai_strerror(int err);

void evutil_secure_rng_and_bytes(void *buf, size_t n);
int  evutil_secure_init();
int  evutil_secure_rng_set_unrandom_device_file(char *fname);
void evutil_secure_rng_add_bytes(const char *dat, size_t datlen);

#endif /* __UTIL_H__ */
