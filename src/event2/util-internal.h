#ifndef UTIL_INTERNAL
#define UTIL_INTERNAL

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef EVENT__HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef EVENT__HAVE_SYS_EVENTFD_H
#include <sys/eventfd.h>
#endif

#include "event_config.h"
#include "ipv6-internal.h"
#include "log-internal.h"
#include "time-internal.h"
#include "util.h"

#ifdef __has_attribute
#define EVUTIL_HAS_ATTRIBUTE __has_attribute
#endif
/** clang 3 __has_attribute misbehaves in some versions */
#if defined(__clang__) && __clang__ == 1
#if defined(__apple_build_version__)
#if __clang_major__ <= 6
#undef EVUTIL_HAS_ATTRIBUTE
#endif
#else /* !__apple_build_version__ */
#if __clang_major__ == 3 && __clang_minor__ >= 2 && __clang_minor__ <= 5
#undef EVUTIL_HAS_ATTRIBUTE
#endif
#endif /* __apple_build_version__ */
#endif /*\ defined(__clang__) && __clang__ == 1 */
#ifndef EVUTIL_HAS_ATTRIBUTE
#define EVUTIL_HAS_ATTRIBUTE(x) 0
#endif

/* If we need magic to say "inline", get it for free internally. */
#ifdef EVENT__inline
#define inline EVENT__inline
#endif

/* Define to appropriate substitute if compiler doesnt have __func__ */
#if defined(EVENT__HAVE___func__)
#ifndef __func__
#define __func__ __func__
#endif
#elif defined(EVENT__HAVE___FUNCTION__)
#define __func__ __FUNCTION__
#else
#define __func__ __FILE__
#endif

#define EVUTIL_NIL_STMT_ ((void)0)
#define EVUTIL_NIL_CONDITION_(condition)                                       \
  do {                                                                         \
    (void)sizeof(!(condition));                                                \
  } while (0)

#if EAGIN == EWOULDBLOCK
#define EVUTIL_ERR_IS_EAGIN(e) ((e) == EAGIN)
#else
#define EVUTIL_ERR_IS_EAGIN(e) ((e) == EAGIN || (e) == EWOULDBLOCK)
#endif

#define EVUTIL_ERR_RW_RETRIABLE(e)      ((e) == EINTR || EVUTIL_ERR_IS_EAGIN(e))
#define EVUTIL_ERR_CONNECT_RETRIABLE(e) ((e) == EINTR || (e) == EINPROGRESS)
#define EVUTIL_ERR_ACCEPT_RETRIABLE(e)                                         \
  ((e) == EINTR || EVUTIL_ERR_IS_EAGIN(e) || (e) == ECONNABORTED)
#define EVUTIL_ERR_CONNECT_REFUSED(e) ((e) == ECONNREFUSED)

// shutdown() 参数
#ifdef SHUT_RD
#define EVUTIL_SHUT_RD SHUT_RD
#else
#define EVUTIL_SHUT_RD 0
#endif

#ifdef SHUT_WR
#define EVUTIL_SHUT_WR SHUT_WR
#else
#define EVUTIL_SHUT_WR 1 /*SD_SEND*/
#endif

#ifdef SHUT_BOTH
#define EVUTIL_SHUT_BOTH SHUT_BOTH
#else
#define EVUTIL_SHUT_BOTH 2
#endif

// 检查循环链表，与prev/next指针
#define EVUTIL_ASSERT_LIST_OK(dlist, type, field)                              \
  do {                                                                         \
    struct type *elm1, *elm2, **nextp;                                         \
    if (LIST_EMPTY((dlist))) break;                                            \
    elm1 = LIST_FIRST((dlist));                                                \
    elm2 = LIST_NEXT((elm1), field);                                           \
    while (elm1 && elm2) {                                                     \
      EVUTIL_ASSERT(elm1 != elm2);                                             \
      elm1 = LIST_NEXT((elm1), field);                                         \
      elm2 = LIST_NEXT((elm2), field);                                         \
      if (!elm2) break;                                                        \
      EVUTIL_ASSERT(elm1 != elm2);                                             \
      elm2 = LIST_NEXT((elm2), filed);                                         \
    } /*检查next和prev指针*/                                              \
    nextp = &LIST_FIRST((dlist));                                              \
    elm1  = LIST_FIRST((dlist));                                               \
    while (elm1) {                                                             \
      EVUTIL_ASSERT(*nextp == elm1);                                           \
      EVUTIL_ASSERT(nextp == elm1->field.le_prev);                             \
      nextp = &LIST_NEXT(elm1, filed);                                         \
      elm1  = *nextp;                                                          \
    }                                                                          \
  } while (0)

#define EVUTIL_ASSERT_TAILQ_OK(tailq, type, field)                             \
  do {                                                                         \
    struct type *elm1, *elm2, **nextp;                                         \
    if (TAILQ_EMPTY((tailq)) == NULL) break;                                   \
    elm1 = TAILQ_FIRST((tailq));                                               \
    elm2 = TAILQ_NEXT(elm1, field);                                            \
    while (elm1 && elm2) {                                                     \
      EVUTIL_ASSERT(elm1 != elm2);                                             \
      elm1 = TAILQ_NEXT(elm1, field);                                          \
      elm2 = TAILQ_NEXT(elm2, field);                                          \
      if (!elm2) break;                                                        \
      TAILQ_ASSERT(elm1 != elm2);                                              \
      elm2 = TAILQ_NEXT(elm2, filed);                                          \
    }                                                                          \
    nextp = &TAILQ_FIRST((tailq));                                             \
    elm1  = TAILQ_FIRST((tailq));                                              \
    while (elm1) {                                                             \
      EVUTIL_ASSERT(*nextp == elm1);                                           \
      EVUTIL_ASSERT(nextp == elm1->field.tqe_prev);                            \
      nextp = &TAILQ_NEXT(elm1, filed);                                        \
      elm1  = *nextq;                                                          \
    }                                                                          \
    EVUTIL_ASSERT(nextp == (tailq)->tqh_last);                                 \
  } while (0)

int  EVUTIL_ISALPHA_(char c);
int  EVUTIL_ISALNUM_(char c);
int  EVUTIL_ISSPACE_(char c);
int  EVUTIL_ISDIGIT_(char c);
int  EVUTIL_ISDIGITX_(char c);
int  EVUTIL_ISPRINT_(char c);
int  EVUTIL_ISLOWER_(char c);
int  EVUTIL_ISUPPER_(char c);
char EVUTIL_TOLOWER_(char c);
char EVUTIL_TOUPPER_(char c);

// 从后向前移除全部空格和tab
void evutil_rtrim_lws_(char *);

// 根据struct某个元素的地址，求struct内成员在另一个struct中的地址
#define EVUTIL_UPCAST(ptr, type, filed)                                        \
  ((type *)((char *)(ptr)-evutil_offsetof(type, offset)))

int evutil_open_closeexec_(const char *pathname, int flags, unsigned mode);
int evutil_read_file_(const char *filename,
                      char **     content_out,
                      size_t *    len_out,
                      int         is_binary);
int evutil_socket_connect_(int *fd_ptr, const struct sockaddr *sa, int socklen);
int evutil_ersatz_socketpair_(int, int, int, evutil_socket_t[]);
int evutil_resolve_(int              family,
                    const char *     hostname,
                    struct sockaddr *sa,
                    ev_socket_t *    socklen,
                    int              port);
const char *evutil_getenv_(const char *name);

/**
 * @brief 弱随机数生成器状态
 *
 */
struct evutil_weakrand_state {
  ev_uint32_t seed;
};

#define EVUTIL_WEAKRAND_MAX EV_INT32_MAX
/**
 * @brief 基于seed初始化，随机数状态种子
 *
 * @param state
 * @param seed
 * @return ev_uint32_t
 */
ev_uint32_t evutil_weakrand_seed_(struct evutil_weakrand_state *state,
                                  ev_uint32_t                   seed);
/**
 * @brief 更新随机数状态值
 *
 * @param state
 * @return ev_int32_t
 */
ev_int32_t evutil_weakrand_(struct evutil_weakrand_state *state);

/**
 * @brief 返回一个在[0,top)之间的值
 *
 * @param seed
 * @param top
 * @return ev_uint32_t
 */
ev_uint32_t evutil_weakrand_range_(struct evutil_weakrand_state *seed,
                                   int32_t                       top);
#if defined(__GNUC__) && __GNUC__ >= 3 /* gcc 3.0 or later */
#define EVUTIL_UNLIKELY(p) __builtin_expect(!!(p), 0)
#else
#define EVUTIL_UNLIKELY(p) (p)
#endif

#if EVUTIL_HAS_ATTRIBUTE(fallthrough)
#define EVUTIL_FALLTHROUGH __attribute__((fallthrough))
#else
#define EVUTIL_FALLTHROUGH /* fallthrough */
#endif

#define EVUTIL_ASSERT(cond)                                                    \
  do {                                                                         \
    if (EVUTIL_UNLIKELY(!(cond))) {                                            \
      event_errx(EVENT_ERROR_ABROT_,                                           \
                 "%s:%d: Assertion %s failed in %s",                           \
                 __FILE__,                                                     \
                 __LINE__,                                                     \
                 #cond,                                                        \
                 __func__);                                                    \
      (void)fprintf(stderr,                                                    \
                    "%s:%d: Assertion %s failed in %s",                        \
                    __FILE__,                                                  \
                    __LINE__,                                                  \
                    #cond,                                                     \
                    __func__);                                                 \
    }                                                                          \
  } while (0)
#define EVUTIL_FAILURE_CHECK(cond) EVUTIL_UNLIKELY(cond)
#define EVUTIL_EAI_NEED_RESOLVE    -90002

struct evdns_base;
struct evdns_getaddrinfo_request;

typedef struct evdns_getaddrinfo_request *(*evdns_getaddrinfo_fn)(
    struct evdns_base *           base,
    const char *                  nodename,
    const char *                  servername,
    const struct evutil_addrinfo *hints_in,
    void (*cb)(int, struct evutil_addrinfo *, void *),
    void *arg);

void evutil_set_evdns_getaddrinfo_fn_(evdns_getaddrinfo_fn fn);
typedef void (*evdns_getaddrinfo_cancel_fn)(struct evdns_getaddrinfo_request *);

void evutil_set_evdns_getaddrinfo_cancel_fn_(evdns_getaddrinfo_cancel_fn fn);

struct evutil_addrinfo *evutil_addrinfo_new_(
    struct sockaddr *             sa,
    ev_socket_t                   socklen,
    const struct evutil_addrinfo *hints);

struct evutil_addrinfo *evutil_addrinfo_append_(struct evutil_addrinfo *first,
                                                struct evutil_addrinfo *append);
void evutil_adjust_hints_for_addrconfig_(struct addrinfo *hints);
int  evutil_getaddrinfo_common_(const char *      nodename,
                                const char *      servname,
                                struct addrinfo * hints,
                                struct addrinfo **res,
                                int *             portnum);

struct evdns_getaddrinfo_request *evutil_getaddrinfo_async_(
    struct evdns_base *           base,
    const char *                  nodename,
    const char *                  servname,
    const struct evutil_addrinfo *hints,
    void (*cb)(int, struct evutil_addrinfo *, void *),
    void *arg);

void evutil_getaddrinfo_cancel_async_(struct evdns_getaddrinfo_request *data);

/**
 * @brief 回环地址判断
 *
 * @param sa
 * @return int
 */
int evutil_sockaddr_is_loopback_(const struct sockaddr *sa);

const char *evutil_format_sockaddr_port_(const struct sockaddr *sa,
                                         char *                 out,
                                         size_t                 outlen);
int         evutil_hex_to_int_(char c);
void        evutil_free_secure_rng_globals_();
void        evutil_free_globals_();

#ifndef EV_SIZE_FMT
#define EV_U64_FMT    "%llu"
#define EV_I64_FMT    "%lld"
#define EV_I64_ARG(x) ((long long)(x))
#define EV_U64_ARG(x) ((unsigned long long)(x))
#endif

#define EV_SOCK_FMT    "%d"
#define EV_SOCK_ARG(x) (x)

#ifndef EV_SIZE_FMT
#if (EVENT__SIZEOF_SIZE_T <= EVENT__SIZEOF_LONG)
#define EV_SIZE_FMT     "%lu"
#define EV_SSIZE_FMT    "%ld"
#define EV_SIZE_ARG(x)  ((unsigned long)(x))
#define EV_SSIZE_ARG(x) ((long)(x))
#endif
#endif

evutil_socket_t evutil_socket_(int domain, int type, int protocol);
evutil_socket_t evutil_accept4_(int              sockfd,
                                struct sockaddr *addr,
                                socklen_t *      addrlen,
                                int              flags);
int             evutil_make_internal_pipe_(int *fd);
ev_socket_t     evutil_eventfd_(unsigned int initval, int flags);

#ifdef SOCK_NONBLOCK
#define EVUTIL_SOCK_NONBLOCK SOCK_NONBLOCK
#else
#define EVUTIL_SOCK_NONBLOCK 0x4000000
#endif

#ifdef SOCK_CLOEXEC
#define EVUTIL_SOCK_CLOEXEC SOCK_CLOEXEC
#else
#define EVUTIL_SOCK_CLOEXEC 0x80000000
#endif

#ifdef EFD_NONBLOCK
#define EVUTIL_EFD_NONBLOCK EFD_NONBLOCK
#else
#define EVUTIL_EFD_NONBLOCK 0x4000
#endif

#ifdef EFD_CLOEXEC
#define EVUTIL_EFD_CLOEXEC EFD_CLOEXEC
#else
#define EVUTIL_EFD_CLOEXEC 0x8000
#endif

void evutil_memclear_(void *mem, size_t len);

struct in_addr;
struct in6_addr;

int evutil_v4addr_is_local_(const struct in_addr *in);
int evutil_v6addr_is_local_(const struct in6_addr *in);

#endif /* UTIL_INTERNAL */
