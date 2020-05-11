#ifndef _PIPE_H_
#define _PIPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <glitter.h>
#include <tscpaq.h>

#include <buffer.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
	size_t bufsz, nbuf;
	tscpaq_t q_in, q_out;
	buffer_t *restrict bufs;
} pipe_t;
	#pragma GCC diagnostic pop

int init_pipe (
	pipe_t *restrict p,
	size_t bufsz, size_t nbuf,
	buffer_t bufs[])
__attribute__ ((leaf, nonnull (1, 4), nothrow, warn_unused_result)) ;

int alloc_pipe (
	pipe_t *restrict p,
	size_t bufsz, size_t nbuf)
__attribute__ ((nonnull (1), nothrow, warn_unused_result)) ;

int free_pipe (pipe_t *restrict p)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

int read_pipe (pipe_t *restrict p, fd_t fd)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

int write_pipe (pipe_t *restrict p, fd_t fd)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

#ifdef __cplusplus
}
#endif

#endif /* _PIPE_H_ */