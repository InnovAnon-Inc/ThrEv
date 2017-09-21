#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdio.h> /* for puts */
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>

#include <glitter.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
	#pragma GCC diagnostic ignored "-Wnested-externs"
	#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#include <ev.h>
	#pragma GCC diagnostic pop

#include <restart.h>
#include <tscpaq.h>

#include <ezudp-server.h>

#ifdef OLD

#include <thpool.h>

typedef struct {
   ev_io io;
   struct ev_loop *loop;
   socket_t s;
   threadpool thpool;
} socket_rd_watcher_t;

typedef struct {
   ev_io io;
   socket_t s;
   char buf[1024];
   ssize_t recv_len;
   struct sockaddr_in si_other;
   socklen_t slen;
   struct ev_loop *loop;
} socket_wr_watcher_t;

static void
socket_wr_cb (EV_P_ ev_io *w_, int revents) {
   socket_wr_watcher_t *w = (socket_wr_watcher_t *) w_;

   w->buf[w->recv_len - 1] = '\0';
   puts (w->buf);

   if (sendto (w->s, w->buf, w->recv_len, 0, (struct sockaddr *) &w->si_other, w->slen) == -1) {
      free (w);
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }

   ev_io_stop (EV_A_ &(w->io));
   free (w);
}

static void thpoolcb (void *arg) {
	/* TODO how to syncronize the event loop? */
   socket_wr_watcher_t *wr_watcher = (socket_wr_watcher_t *) arg;
   ev_io_init (&(wr_watcher->io), socket_wr_cb, wr_watcher->s, EV_WRITE);
   ev_io_start (wr_watcher->loop, (ev_io *) wr_watcher);
   puts ("thpoolcb()");
}

static void
socket_rd_cb (EV_P_ ev_io *w_, int revents) {
   socket_rd_watcher_t *w = (socket_rd_watcher_t *) w_;

   socket_wr_watcher_t *wr_watcher = (socket_wr_watcher_t *) malloc (sizeof (socket_wr_watcher_t));
   if (wr_watcher == NULL) {
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }

   wr_watcher->slen = sizeof (wr_watcher->si_other);

   wr_watcher->recv_len = recvfrom (w->s, wr_watcher->buf, sizeof (wr_watcher->buf), 0, (struct sockaddr *) &wr_watcher->si_other, &wr_watcher->slen);
   if (wr_watcher->recv_len == -1) {
      free (wr_watcher);
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }

   wr_watcher->s = w->s;
   wr_watcher->loop = w->loop;

   thpool_add_work (w->thpool, thpoolcb, wr_watcher);
}
/*
void thpoolcb (void *arg) {
   socket_t s = (socket_t) arg;

   struct ev_loop *loop = EV_DEFAULT;

   socket_rd_watcher_t rd_watcher;
   rd_watcher.loop = loop;
   rd_watcher.s = s;
   ev_io_init (&(rd_watcher.io), socket_rd_cb, s, EV_READ);
   ev_io_start (loop, (ev_io *) &rd_watcher);

   ev_run (loop, 0);
}
*/
static int ezthpoolcb (threadpool thpool, socket_t s) {
   /*return thpool_add_work (
      thpool, thpoolcb, (void *) s);*/

   struct ev_loop *loop = EV_DEFAULT;

   socket_rd_watcher_t rd_watcher;
   rd_watcher.loop = loop;
   rd_watcher.s = s;
   rd_watcher.thpool = thpool;
   ev_io_init (&(rd_watcher.io), socket_rd_cb, s, EV_READ);
   ev_io_start (loop, (ev_io *) &rd_watcher);

   ev_run (loop, 0);
   return 0;
}

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


	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   size_t n;
   char *restrict buf;
} buffer_t;
	#pragma GCC diagnostic pop

__attribute__ ((nonnull (1, 2), nothrow))
static void init_buffer (
   buffer_t *restrict buffer,
   char *restrict buf) {
   /*buffer->n = 0;*/
   buffer->buf = buf;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int alloc_buffer (
   buffer_t *restrict buffer,
   size_t bufsz) {
   size_t i;
   char *restrict buf = malloc (bufsz);
   error_check (buf == NULL) return -1;
   init_buffer (buffer, buf);
   return 0;
}

__attribute__ ((nonnull (1), nothrow))
static void free_buffer (buffer_t *restrict buffer) {
   free (buffer->buf);
}



	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   size_t bufsz, nbuf;
   tscpaq_t q_in, q_out;
   buffer_t *restrict bufs;
} pipe_t;
	#pragma GCC diagnostic pop

__attribute__ ((nonnull (1, 4), nothrow, warn_unused_result))
static int init_pipe (
   pipe_t *restrict p,
   size_t bufsz, size_t nbuf,
   buffer_t *restrict bufs) {
   size_t i;
   p->bufsz = bufsz;
   p->nbuf  = nbuf;
   p->bufs  = bufs;

   for (i = 0; i != nbuf; i++)
      error_check (tscpaq_enqueue (
      &(p->q_in),
      bufs + i) != 0)
      return -4;

   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int alloc_pipe (
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

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int free_pipe (pipe_t *restrict p) {
   size_t i;
   error_check (tscpaq_free_queue (&(p->q_in)) != 0) return -1;
   error_check (tscpaq_free_queue (&(p->q_out)) != 0) return -2;
   for (i = 0; i != p->nbuf; i++)
      free_buffer (p->bufs + i);
   free (p->bufs);
}

TODO (rw_pipe_common ())

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int read_pipe (pipe_t *restrict p, fd_t fd) {
   buffer_t *restrict buf;
   ssize_t n;

   error_check (tscpaq_dequeue (
      &(p->q_in), (void const *restrict *restrict) &buf) != 0)
      return -1;

   n = r_read (fd, buf->buf, p->bufsz - 1);

   error_check (n < 0) return -2;

   buf->buf[(size_t) n] = '\0';
   buf->n = (size_t) n + 1;

   if (n == 0) return /*0*/ -1;

   error_check (tscpaq_enqueue (&(p->q_out), buf) != 0)
      return -3;

   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int write_pipe (pipe_t *restrict p, fd_t fd) {
   buffer_t *restrict buf;
   ssize_t n;

   error_check (tscpaq_dequeue (
      &(p->q_out), (void const *restrict *restrict) &buf) != 0)
      return -1;

   n = r_write (fd, buf->buf, buf->n);

   error_check (n < 0) return -2;

   buf->n = (size_t) n;

   if (n == 0) return /*0*/ -1;

   error_check (tscpaq_enqueue (&(p->q_in), buf) != 0)
      return -3;

   return 0;
}



	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   pipe_t *restrict in;
   pipe_t *restrict out;
} io_t;
	#pragma GCC diagnostic pop

__attribute__ ((nonnull (1, 2, 3/*, 4*/), nothrow))
static void init_io (
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
static int alloc_io (
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

__attribute__ ((nonnull (1/*, 2*/), nothrow, warn_unused_result))
static int free_io (io_t *restrict dest/*, io_t *restrict src*/) {
   error_check (free_pipe (dest->in) != 0) return -1;
   error_check (free_pipe (dest->out) != 0) return -2;
   free (dest->in);
   free (dest->out);
   return 0;
}









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

__attribute__ ((nonnull (1), nothrow))
static void ev_read_cb (EV_P_ ev_io *restrict _w, int revents) {
   rd_watcher_t *restrict w = (rd_watcher_t *restrict) _w;
   TODO (check revents)
   error_check (read_pipe (w->in, w->fd)  != 0) return;
}
__attribute__ ((nonnull (1), nothrow))
static void ev_write_cb (EV_P_ ev_io *restrict _w, int revents) {
   wr_watcher_t *restrict w = (wr_watcher_t *restrict) _w;
   TODO (check revents)
   error_check (write_pipe (w->out, w->fd) != 0) return;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *io_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;
   pipe_t *restrict in;
   pipe_t *restrict out;

   TODO (async should be optional. tradional io should also be possible)
   struct ev_loop *restrict loop = EV_DEFAULT;

   rd_watcher_t rd_watcher;
   wr_watcher_t wr_watcher;

   rd_watcher.in  = arg->in;
   wr_watcher.out = arg->out;

   rd_watcher.fd = STDIN_FILENO;
   wr_watcher.fd = STDOUT_FILENO;
   ev_io_init (&(rd_watcher.io), rd_cb, rd_watcher.fd, EV_READ);
   ev_io_start (loop, (ev_io *) &rd_watcher);

   ev_io_init (&(wr_watcher.io), wr_cb, wr_watcher.fd, EV_WRITE);
   ev_io_start (loop, (ev_io *) &wr_watcher);

   ev_run (loop, 0);
   return NULL;
}








#ifndef min
#define min(A, B) ((A) < (B) ? (A) : (B))
#endif




__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *worker_thread_cb (void *restrict _arg) {
   io_t *restrict arg = (io_t *restrict) _arg;
   pipe_t *restrict in;
   pipe_t *restrict out;
   in  = arg->in;
   out = arg->out;
   /*in  = arg->out;
   out = arg->in;*/
   while (true) {
      buffer_t const *restrict buf_in;
      buffer_t *restrict buf_out;

      error_check (tscpaq_dequeue (&(in->q_out), (void const *restrict *restrict) &buf_in)   != 0) {
         TODO (kill other thread);
         return NULL;
      }

      error_check (tscpaq_dequeue (&(out->q_in), (void const *restrict *restrict) &buf_out)  != 0) {
         TODO (kill other thread);
         return NULL;
      }

      TODO (something else)

      if (buf_in->n == 0)
         return NULL;

      TODO (something else... something with threadpools)
      memcpy (buf_out->buf, buf_in->buf, min (buf_in->n, out->bufsz));
      /*memcpy (buf_out->buf, buf_in->buf, buf_in->n);*/
      buf_out->n = min (buf_in->n, out->bufsz);


      error_check (tscpaq_enqueue (&(out->q_out), buf_out) != 0) {
         TODO (kill other thread);
         return NULL;
      }

      error_check (tscpaq_enqueue (&(in->q_in),   buf_in)  != 0) {
         TODO (kill other thread);
         return NULL;
      }

   }
   return NULL;
}

__attribute__ ((nothrow))
int main (void) {
   io_t dest/*, src*/;
   size_t in_bufsz = 3;
   size_t in_nbuf  = 3;
   size_t out_bufsz = 3;
   size_t out_nbuf  = 3;
   pthread_t io_thread, worker_thread;
   buffer_t *restrict buf_in;
   buffer_t *restrict buf_out;
   error_check (alloc_io (&dest, /*&src,*/
      in_bufsz, in_nbuf, out_bufsz, out_nbuf) != 0) return EXIT_FAILURE;

   pthread_create (&io_thread, NULL, io_thread_cb, &dest);
   pthread_create (&worker_thread, NULL, worker_thread_cb, /*&src*/ &dest);
   pthread_join (io_thread, NULL);
   pthread_join (worker_thread, NULL);

   error_check (free_io (&dest/*, &src*/) != 0) return EXIT_FAILURE;

   return EXIT_SUCCESS;
}
