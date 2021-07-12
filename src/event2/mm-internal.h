#ifndef __MM_INTERNAL_H__
#define __MM_INTERNAL_H__

#include <sys/types.h>

#include <cstddef>

#ifndef EVENT__DISABLE_MM_REPLACEMENT

/**
 * @brief 返回未初始化的内存
 *
 * @param sz
 * @return void*
 */
void *event_mm_malloc_(size_t sz);
/**
 * @brief 返回初始化为0的内存快
 *
 * @param sz
 * @return void*
 */
void *event_mm_calloc_(size_t count, size_t sz);
/**
 * @brief 复制一个字符串
 *
 * @param str
 * @return char*
 */
char *event_mm_strdup_(const char *str);
void *event_mm_realloc_(void *p, size_t sz);
void  event_mm_free_(void *p);

#define mm_malloc(sz)          event_mm_malloc_(sz)
#define mm_calloc(count, size) event_mm_calloc_((count), (size))
#define mm_strdup(s)           event_mm_strdup_(s)
#define mm_realloc(p, size)    event_mm_realloc_(p, size)
#define mm_free(p)             event_mm_free_(p)
#else
#define mm_malloc(sz)         malloc(sz)
#define mm_calloc(cout, size) calloc((cout), (size))
#define mm_strdup(s)          strdup(s)
#define mm_realloc(p, sz)     realloc((p), (sz))
#define mm_free(p)            free(p)
#endif

#endif /* __MM_INTERNAL_H__ */
