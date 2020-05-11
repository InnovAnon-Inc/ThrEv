#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <glitter.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   size_t n;
   char *restrict buf;
} buffer_t;
	#pragma GCC diagnostic pop

void init_buffer (
   buffer_t *restrict buffer,
   char *restrict buf)
__attribute__ ((leaf, nonnull (1, 2), nothrow)) ;

int alloc_buffer (
   buffer_t *restrict buffer,
   size_t bufsz)
__attribute__ ((nonnull (1), nothrow, warn_unused_result)) ;

void free_buffer (buffer_t const *restrict buffer)
__attribute__ ((leaf, nonnull (1), nothrow)) ;

#ifdef __cplusplus
}
#endif

#endif /* _BUFFER_H_ */