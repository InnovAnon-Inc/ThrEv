#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdlib.h>

#include <io.h>

__attribute__ ((leaf, nonnull (1, 2, 3/*, 4*/), nothrow))
void init_io (
   io_t *restrict io_in,
   /*io_t *restrict io_out,*/
   pipe_t *restrict in,
   pipe_t *restrict out) {
   io_in->in  = in;
   io_in->out = out;
   /*io_out->in  = out;
   io_out->out = in;*/
}

__attribute__ ((nonnull (1/*, 2*/), nothrow, warn_unused_result))
int alloc_io (
   io_t *restrict dest,
   /*io_t *restrict src,*/
   size_t in_bufsz,  size_t in_nbuf,
   size_t out_bufsz, size_t out_nbuf) {
   pipe_t *restrict in;
   pipe_t *restrict out;

   in  = malloc (sizeof (pipe_t));
   error_check (in == NULL) return -1;
   out = malloc (sizeof (pipe_t));
   error_check (out == NULL) {
      free (in);
      return -2;
   }

   init_io (dest, /*src,*/ in, out);

   error_check (alloc_pipe (in, in_bufsz, in_nbuf) != 0) {
      free (out);
      free (in);
      return -3;
   }
   error_check (alloc_pipe (out, out_bufsz, out_nbuf) != 0) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-result"
      (void) free_pipe (in);
	#pragma GCC diagnostic pop
      free (out);
      free (in);
      return -4;
   }

   return 0;
}

__attribute__ ((leaf, nonnull (1/*, 2*/), nothrow, warn_unused_result))
int free_io (io_t const *restrict dest/*, io_t *restrict src*/) {
   error_check (free_pipe (dest->in) != 0) return -1;
   error_check (free_pipe (dest->out) != 0) return -2;
   free (dest->in);
   free (dest->out);
   return 0;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
int rw_io (io_t const *restrict arg, fd_t rd, fd_t wr) {
   while (true) {
      error_check (read_pipe  (arg->in,  rd) != 0) return -1;
      error_check (write_pipe (arg->out, wr) != 0) return -2;
   }
   /*return 0;*/
   __builtin_unreachable ();
}

__attribute__ ((leaf, nonnull (1, 2), nothrow, warn_unused_result))
int worker_io (
   io_t const *restrict arg,
   worker_io_cb_t cb, void *restrict cbarg) {
   pipe_t *restrict in;
   pipe_t *restrict out;
   in  = arg->in;
   out = arg->out;
   /*in  = arg->out;
   out = arg->in;*/
   while (true) {
      buffer_t const *restrict buf_in;
      buffer_t *restrict buf_out;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
      error_check (tscpaq_dequeue (&(in->q_out), (void const *restrict *restrict) &buf_in)   != 0) {
	#pragma GCC diagnostic pop
         TODO (kill other thread);
         return -1;
      }

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
      error_check (tscpaq_dequeue (&(out->q_in), (void const *restrict *restrict) &buf_out)  != 0) {
	#pragma GCC diagnostic pop
         TODO (kill other thread);
         return -2;
      }

      if (buf_in->n == 0) {
         TODO (kill other thread)
         return -3;
      }

      buf_out->n = out->bufsz;
      error_check (cb (buf_out, buf_in, cbarg) != 0) {
         TODO (kill other thread)
         return -4;
      }
      /*buf_out->n = min (buf_in->n, out->bufsz);
      memcpy (buf_out->buf, buf_in->buf, buf_out->n);*/
      /*memcpy (buf_out->buf, buf_in->buf, buf_in->n);*/

      error_check (tscpaq_enqueue (&(out->q_out), buf_out) != 0) {
         TODO (kill other thread);
         return -5;
      }
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
      error_check (tscpaq_enqueue (&(in->q_in),   buf_in)  != 0) {
	#pragma GCC diagnostic pop
         TODO (kill other thread);
         return -6;
      }
   }
   /*return 0;*/
   __builtin_unreachable ();
}
