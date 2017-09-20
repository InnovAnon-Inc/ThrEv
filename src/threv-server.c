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






typedef struct {

} io_thread_t;

void *io_thread_cb (void *_arg) {
   io_thread_t *arg = (io_thread_t *) _arg;
   /* alloc buf, read into buf, enqueue buf */
   /* dequeue buf, write from buf, dealloc buf */
   return NULL;
}

int main (void) {
   /*
   const int err = ezudp_server (1234, INADDR_ANY, ezudpcb, NULL);
   if (err != 0) {
      fprintf (stderr, "err:%d\n", err);
      return EXIT_FAILURE;
   }*/

   pthread_t io_thread;
   io_thread_t io_thread_arg;

   pthread_create (&io_thread, NULL, io_thread_cb, io_thread_arg);
   work_thread_cb ();

   return EXIT_SUCCESS;
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

   TODO (delete this)
   for (i = 0; i != bufsz; i++) buf[i] = '\0';

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






typedef struct {
   pipe_t in, out;
} io_thread_cb2_t;

typedef __attribute__ ((nonnull (2), /*nothrow,*/ warn_unused_result))
ssize_t (*io_thread_cb_common_cb_t) (fd_t, buffer_t *, size_t);

__attribute__ ((nonnull (2), /*nothrow,*/))
static ssize_t read_wrapper (
   fd_t fd,
   buffer_t *restrict buf,
   size_t bufsz) {
   ssize_t n = r_read (fd, buf->buf, bufsz - 1);
   error_check (n < 0) return -1;
   buf->buf[n] = '\0';
   printf ("buf:%s\n", buf->buf); fflush (stdout);
   buf->n = (size_t) n + 1;
   return n;
}

__attribute__ ((nonnull (2), /*nothrow,*/))
static ssize_t write_wrapper (
   fd_t fd,
   buffer_t *restrict buf,
   size_t bufsz) {
   ssize_t n;
   n = r_write (fd, buf->buf, buf->n);
   error_check (n <= 0) return -1;
   buf->n = (size_t) n;
   return n;
}

__attribute__ ((nonnull (1, 2, 4), nothrow, warn_unused_result))
static int io_thread_cb_common (
   tscpaq_t *restrict q_in,
   tscpaq_t *restrict q_out,
   size_t bufsz,
   io_thread_cb_common_cb_t cb,
   fd_t fd) {
   buffer_t *restrict buf;
   ssize_t n;
   /*while (true) {*/
      error_check (tscpaq_dequeue (
         q_in, (void const *restrict *restrict) &buf) != 0)
         return -1;
      n = cb (fd, buf, bufsz);
      if (n == 0) return /*0*/ -1;
      error_check (n < 0) return -2;
      error_check (tscpaq_enqueue (q_out, buf) != 0)
         return -3;
   /*}*/
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int io_thread_cb_rd (
   pipe_t *restrict arg_in) {
   return io_thread_cb_common (
      &(arg_in->q_in), &(arg_in->q_out), arg_in->bufsz,
      read_wrapper, STDIN_FILENO);
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int io_thread_cb_wr (
   pipe_t *restrict arg_out) {
   return io_thread_cb_common (
      &(arg_out->q_out), &(arg_out->q_in), arg_out->bufsz /* *ret*/,
      write_wrapper, STDOUT_FILENO);
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *io_thread_cb (void *_arg) {
   io_thread_cb2_t *restrict arg = (io_thread_cb2_t *restrict) _arg;
   pipe_t *restrict arg_in;
   pipe_t *restrict arg_out;
   arg_in  = &(arg->in);
   arg_out = &(arg->out);
   while (true) {
      error_check (io_thread_cb_rd ((void *) arg_in)  != 0) return NULL;
      /*if (arg_in == 0) return NULL;*/
      error_check (io_thread_cb_wr ((void *) arg_out) != 0) return NULL;
   }
   return NULL;
}

#ifndef min
#define min(A, B) ((A) < (B) ? (A) : (B))
#endif

__attribute__ ((nothrow))
int main (void) {
   pipe_t *restrict args_in;
   pipe_t *restrict args_out;
   io_thread_cb2_t args;
   pthread_t io_thread;

   args_in  = &(args.in);
   args_out = &(args.out);

   error_check (alloc_pipe (args_in,  (size_t)   3, (size_t) 3) != 0) return EXIT_FAILURE;
   error_check (alloc_pipe (args_out, (size_t)   3, (size_t) 3) != 0) return EXIT_FAILURE;

   pthread_create (&io_thread, NULL, io_thread_cb, (void *) &args);

   while (true) { /* while other thread is alive*/
      buffer_t const *restrict buf_in;
      buffer_t *restrict buf_out;

      error_check (tscpaq_dequeue (&(args_in->q_out), (void const *restrict *restrict) &buf_in)   != 0) {
         TODO (kill other thread);
         break;
      }
      error_check (tscpaq_dequeue (&(args_out->q_in), (void const *restrict *restrict) &buf_out)  != 0) {
         TODO (kill other thread);
         break;
      }
      TODO (something else)

      memcpy (buf_out->buf, buf_in->buf, min (buf_in->n, buf_out->n));

      error_check (tscpaq_enqueue (&(args_out->q_out), buf_out) != 0) {
         TODO (kill other thread);
         break;
      }
      error_check (tscpaq_enqueue (&(args_in->q_in),   buf_in)  != 0) {
         TODO (kill other thread);
         break;
      }
   }
   /*__builtin_unreachable ();*/

   TODO (pthread join)
   error_check (free_pipe (args_out) != 0) return EXIT_FAILURE;
   error_check (free_pipe (args_in)  != 0) return EXIT_FAILURE;
   return EXIT_SUCCESS;
   /*return EXIT_FAILURE;*/
}

