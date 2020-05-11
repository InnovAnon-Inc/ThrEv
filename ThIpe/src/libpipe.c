#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdlib.h>

#include <restart.h>
#include <tscpaq.h>

#include <pipe.h>

__attribute__ ((leaf, nonnull (1, 4), nothrow, warn_unused_result))
int init_pipe (
   pipe_t *restrict p,
   size_t bufsz, size_t nbuf,
   buffer_t bufs[]) {
   size_t i;
   p->bufsz = bufsz;
   p->nbuf  = nbuf;
   p->bufs  = bufs;

   TODO (we probably do not need synchronization here)
   for (i = 0; i != nbuf; i++)
      error_check (tscpaq_enqueue (
      &(p->q_in),
      bufs + i) != 0)
      return -4;

   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
int alloc_pipe (
   pipe_t *restrict p,
   size_t bufsz, size_t nbuf) {
   size_t i;
   buffer_t *restrict bufs;

   bufs = malloc (nbuf * sizeof (buffer_t));
   error_check (bufs == NULL) return -1;

   error_check (tscpaq_alloc_queue (&(p->q_in), nbuf + 1) != 0) {
      free (bufs);
      return -2;
   }
   error_check (tscpaq_alloc_queue (&(p->q_out), nbuf + 1) != 0) {
      free (bufs);
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-result"
      (void) tscpaq_free_queue (&(p->q_in));
	#pragma GCC diagnostic pop
      return -3;
   }

   /*error_check (init_pipe (p, bufsz, nbuf, bufs) != 0) return -2;*/

	TODO (move for-j-loop out of here)
	#pragma GCC ivdep
   for (i = 0; i != nbuf; i++)
      error_check (alloc_buffer (bufs + i, bufsz) != 0) {
         size_t j;
         for (j = 0; j != i; j++) free_buffer (bufs + j);
         free (bufs);
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-result"
         (void) tscpaq_free_queue (&(p->q_out));
         (void) tscpaq_free_queue (&(p->q_in));
	#pragma GCC diagnostic pop
         return -4;
      }

   error_check (init_pipe (p, bufsz, nbuf, bufs) != 0) return -2;

   return 0;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
int free_pipe (pipe_t *restrict p) {
   size_t i;
   error_check (tscpaq_free_queue (&(p->q_in)) != 0) return -1;
   error_check (tscpaq_free_queue (&(p->q_out)) != 0) return -2;
	#pragma GCC ivdep
   for (i = 0; i != p->nbuf; i++)
      free_buffer (p->bufs + i);
   free (p->bufs);
}

TODO (rw_pipe_common ())

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
int read_pipe (pipe_t *restrict p, fd_t fd) {
   buffer_t *restrict buf;
   ssize_t n;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   error_check (tscpaq_dequeue (
      &(p->q_in), (void const *restrict *restrict) &buf) != 0)
	#pragma GCC diagnostic pop
      return -1;

   n = r_read (fd, buf->buf, p->bufsz - 1);

   error_check (n < 0) return -2;

   TODO (this belongs in client code)
   buf->buf[(size_t) n] = '\0';
   buf->n = (size_t) n + 1;

   if (n == 0) return /*0*/ -1;

   error_check (tscpaq_enqueue (&(p->q_out), buf) != 0)
      return -3;

   return 0;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
int write_pipe (pipe_t *restrict p, fd_t fd) {
   buffer_t *restrict buf;
   ssize_t n;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   error_check (tscpaq_dequeue (
      &(p->q_out), (void const *restrict *restrict) &buf) != 0)
	#pragma GCC diagnostic pop
      return -1;

   n = r_write (fd, buf->buf, buf->n);

   error_check (n < 0) return -2;

   buf->n = (size_t) n;

   if (n == 0) return /*0*/ -1;

   error_check (tscpaq_enqueue (&(p->q_in), buf) != 0)
      return -3;

   return 0;
}
