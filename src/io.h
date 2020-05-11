#ifndef _IO_H_
#define _IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <glitter.h>
#include <tscpaq.h>

#include <pipe.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   pipe_t *restrict in;
   pipe_t *restrict out;
} io_t;
	#pragma GCC diagnostic pop

void init_io (
	io_t *restrict io_in,
   /*io_t *restrict io_out,*/
   pipe_t *restrict in,
   pipe_t *restrict out)
__attribute__ ((leaf, nonnull (1, 2, 3/*, 4*/), nothrow)) ;

int alloc_io (
   io_t *restrict dest,
   /*io_t *restrict src,*/
   size_t in_bufsz,  size_t in_nbuf,
   size_t out_bufsz, size_t out_nbuf)
__attribute__ ((nonnull (1/*, 2*/), nothrow, warn_unused_result)) ;

int free_io (io_t const *restrict dest/*, io_t *restrict src*/)
__attribute__ ((leaf, nonnull (1/*, 2*/), nothrow, warn_unused_result)) ;

int rw_io (io_t const *restrict arg, fd_t rd, fd_t wr)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

typedef __attribute__ ((nonnull (1, 2), /*nothrow,*/ warn_unused_result))
int (*worker_io_cb_t) (
   buffer_t *restrict buf_out,
   buffer_t const *restrict buf_in,
   void *restrict arg) ;

int worker_io (
   io_t const *restrict arg,
   worker_io_cb_t cb, void *restrict cbarg)
__attribute__ ((leaf, nonnull (1, 2), nothrow, warn_unused_result)) ;

#ifdef __cplusplus
}
#endif

#endif /* _IO_H_ */