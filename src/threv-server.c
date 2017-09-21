#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdio.h> /* for puts */
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <glitter.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
	#pragma GCC diagnostic ignored "-Wnested-externs"
	#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#include <ev.h>
	#pragma GCC diagnostic pop

#include <restart.h>
#include <io.h>

#include <ezudp-server.h>

#ifdef OLD

#include <thpool.h>

int ezthpool (int (*cb) (threadpool, socket_t), socket_t arg) {
	threadpool thpool = thpool_init (2);

	if (cb (thpool, arg) != 0) {
		thpool_destroy (thpool);
		return -1;
	}

	thpool_wait (thpool);
	thpool_destroy (thpool);
	return 0;
}

static int ezudpcb (socket_t s, void *unused) {
   return ezthpool (ezthpoolcb, s);
}






#endif

/*
 read from socket
 enqueue data

 dequeue data
 process data
 enqueue result

 dequeue result
 write to socket


 check whether results can be dequeued
 if so, dequeue result
        write to socket
 else
 check whether data can be enqueued
 if so, read from socket
        enqueue data
 */








/*#define DO_ASYNC 1*/

#ifndef DO_ASYNC
__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *io_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;
   error_check (rw_io (arg, STDIN_FILENO, STDOUT_FILENO) != 0) return NULL;
   return NULL;
}
#else
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   ev_io io;
   fd_t fd;
   pipe_t *restrict in;
} rd_watcher_t;
	#pragma GCC diagnostic pop

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   ev_io io;
   fd_t fd;
   pipe_t *restrict out;
} wr_watcher_t;
	#pragma GCC diagnostic pop

TODO (ev_rw_cb_common ())

__attribute__ ((nonnull (1), nothrow))
static void ev_read_cb (EV_P_ ev_io *restrict _w, int revents) {
   rd_watcher_t *restrict w = (rd_watcher_t *restrict) _w;
   TODO (check revents)
   error_check (read_pipe (w->in, w->fd)  != 0) {
      TODO (stop ev loop)
      return;
   }
}
__attribute__ ((nonnull (1), nothrow))
static void ev_write_cb (EV_P_ ev_io *restrict _w, int revents) {
   wr_watcher_t *restrict w = (wr_watcher_t *restrict) _w;
   TODO (check revents)
   error_check (write_pipe (w->out, w->fd) != 0) {
      TODO (stop ev loop)
      return;
   }
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *rd_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;

   struct ev_loop *restrict loop = EV_DEFAULT;

   rd_watcher_t rd_watcher;

   rd_watcher.in  = arg->in;

   rd_watcher.fd = STDIN_FILENO;
   ev_io_init (&(rd_watcher.io), ev_read_cb, rd_watcher.fd, EV_READ);
   ev_io_start (loop, (ev_io *) &rd_watcher);

   ev_run (loop, 0);
   return NULL;
}
__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *wr_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;

   struct ev_loop *restrict loop = ev_loop_new (EVFLAG_AUTO);

   wr_watcher_t wr_watcher;

   wr_watcher.out = arg->out;

   wr_watcher.fd = STDOUT_FILENO;

   ev_io_init (&(wr_watcher.io), ev_write_cb, wr_watcher.fd, EV_WRITE);
   ev_io_start (loop, (ev_io *) &wr_watcher);

   ev_run (loop, 0);
   return NULL;
}
#endif






TODO (this macro has been moved to glitter.h)
#ifndef min
#define min(A, B) ((A) < (B) ? (A) : (B))
#endif




__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result))
static int worker_thread_cb_cb (
   buffer_t *restrict buf_out,
   buffer_t const *restrict buf_in,
   void *restrict unused) {
   buf_out->n = min (buf_in->n, buf_out->n);
   (void) memcpy (buf_out->buf, buf_in->buf, buf_out->n);
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *worker_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;
   error_check (worker_io (arg, worker_thread_cb_cb, NULL) != 0) return NULL;
   return NULL;
}

__attribute__ ((nothrow))
int main (void) {
   io_t dest/*, src*/;
   size_t in_bufsz = 3;
   size_t in_nbuf  = 3;
   size_t out_bufsz = 3;
   size_t out_nbuf  = 3;
#ifndef DO_ASYNC
   pthread_t io_thread;
#else
   pthread_t rd_thread;
   pthread_t wr_thread;
#endif
   pthread_t worker_thread;
   buffer_t *restrict buf_in;
   buffer_t *restrict buf_out;
   error_check (alloc_io (&dest, /*&src,*/
      in_bufsz, in_nbuf, out_bufsz, out_nbuf) != 0) return EXIT_FAILURE;

#ifndef DO_ASYNC
   pthread_create (&io_thread, NULL, io_thread_cb, &dest);
#else
   pthread_create (&rd_thread, NULL, rd_thread_cb, &dest);
   pthread_create (&wr_thread, NULL, wr_thread_cb, &dest);
#endif
   pthread_create (&worker_thread, NULL, worker_thread_cb, /*&src*/ &dest);
#ifndef DO_ASYNC
   pthread_join (io_thread, NULL);
#else
   pthread_join (rd_thread, NULL);
   pthread_join (wr_thread, NULL);
#endif
   pthread_join (worker_thread, NULL);

   error_check (free_io (&dest/*, &src*/) != 0) return EXIT_FAILURE;

   return EXIT_SUCCESS;
}
