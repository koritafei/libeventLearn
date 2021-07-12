#ifndef LOG_INTERNAL
#define LOG_INTERNAL

#include "util.h"

#ifdef __GNUC__
#define EV_CHECK_FMT(a, b) __attribute__((format(printf, a, b)))
#define EV_NORETURN        __attribute__((noreturn))
#else
#define EV_CHECK_FMT(a, b)
#define EV_NORETURN
#endif

#define EVENT_ERROR_ABROT_ ((int)0xdeaddead)

#if !defined(EVENT__DISABLE_DEBUG_MODE) || defined(USE_DEBUG)
#define EVENT_DEBUG_LOGGING_ENABLED
#endif

#ifdef EVENT_DEBUG_LOGGING_ENABLED
#define event_debug_get_logging_mask_() (event_debug_logging_mask_)
#else
#define event_debug_get_logging_mask_() (0)
#endif

void event_err(int eval, const char *fmt, ...) EV_CHECK_FMT(2, 3) EV_NORETURN;
void event_warn(const char *fmt, ...) EV_CHECK_FMT(1, 2);
void event_socket_error(int eval, evutil_socket_t sock, const char *fmt, ...)
    EV_CHECK_FMT(3, 4) EV_NORETURN;
void event_socket_warn(evutil_socket_t sock, const char *fmt, ...)
    EV_CHECK_FMT(2, 3);
void event_errx(int eval, const char *fmt, ...) EV_CHECK_FMT(2, 3) EV_NORETURN;
void event_warnx(const char *fmt, ...) EV_CHECK_FMT(1, 2);
void event_msgx(const char *fmt, ...) EV_CHECK_FMT(1, 2);
void event_debugx_(const char *fmt, ...) EV_CHECK_FMT(1, 2);
void event_logv_(int severity, const char *errstr, const char *fmt, va_list va)
    EV_CHECK_FMT(3, 0);

#ifdef EVENT_DEBUG_LOGGING_ENABLED
#define event_debug(x)                                                         \
  do {                                                                         \
    if (event_debug_get_logging_mask_()) {                                     \
      event_debugx_ x;                                                         \
    }                                                                          \
  } while (0)
#else
#define event_debug(x) ((void)0)
#endif

#endif /* LOG_INTERNAL */
