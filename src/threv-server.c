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


__attribute__ ((const, nonnull (1), nothrow, returns_nonnull, warn_unused_result))
static char const *get_buf (
   char const bufs[],
   size_t i, size_t bufsz, size_t nbuf) {
   return bufs + i * bufsz;
}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   size_t bufsz, nbuf;
   tscpaq_t q_in, q_out;
   char *restrict bufs;
} io_thread_cb_t;
	#pragma GCC diagnostic pop

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int init_io_thread_cb (
   io_thread_cb_t *restrict args, size_t bufsz, size_t nbuf) {
   size_t i;
   args->bufsz = bufsz;
   args->nbuf  = nbuf;

   args->bufs = (char *restrict) malloc (args->nbuf * args->bufsz);
   error_check (args->bufs == NULL) return -1;

   error_check (tscpaq_alloc_queue (&(args->q_in), args->nbuf) != 0) {
      free (args->bufs);
      return -2;
   }

   error_check (tscpaq_alloc_queue (&(args->q_out), args->nbuf) != 0) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-result"
      (void) tscpaq_free_queue (&(args->q_in));
	#pragma GCC diagnostic pop
      free (args->bufs);
      return -3;
   }

   for (i = 0; i != nbuf - 1; i++)
      error_check (tscpaq_enqueue (
         &(args->q_in),
         get_buf (args->bufs, i, bufsz, nbuf)) != 0)
         return -4;

   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int free_io_thread_cb (io_thread_cb_t *restrict arg) {
   error_check (tscpaq_free_queue (&(arg->q_out)) != 0) return -1;
   error_check (tscpaq_free_queue (&(arg->q_in))  != 0) return -2;
   free (arg->bufs);
   return 0;
}

typedef struct {
   io_thread_cb_t in, out;
} io_thread_cb2_t;

typedef __attribute__ ((nonnull (2), nothrow, warn_unused_result))
ssize_t (*io_thread_cb_common_cb_t) (fd_t, void *, size_t);

__attribute__ ((nonnull (1, 2, 3, 5), nothrow, warn_unused_result))
static int io_thread_cb_common (
   size_t *restrict ret,
   tscpaq_t *restrict q_in,
   tscpaq_t *restrict q_out,
   size_t bufsz,
   io_thread_cb_common_cb_t cb,
   fd_t fd) {
   char *restrict buf;
   ssize_t n;
   /*while (true) {*/
      error_check (tscpaq_dequeue (
         q_in, (void const *restrict *restrict) &buf) != 0)
         return -1;
      n = cb (fd, buf, bufsz);
      if (n == 0) return 0 /*-1*/;
      error_check (n < 0) return -2;
      *ret = (size_t) n;
      error_check (tscpaq_enqueue (q_out, buf) != 0)
         return -3;
   /*}*/
   return 0;
}

__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result))
static int io_thread_cb_rd (
   size_t *restrict ret,
   io_thread_cb_t *restrict arg_in) {
   return io_thread_cb_common (
      ret,
      &(arg_in->q_in), &(arg_in->q_out), arg_in->bufsz,
      r_read, STDIN_FILENO);
#ifdef OTHER
   char *restrict buf_in;
   ssize_t rd;
   /*while (true) {*/
      /* reader */
      error_check (tscpaq_dequeue (
         &(arg_in->q_in), (void const *restrict *restrict) &buf_in) != 0)
         return -1;
      rd = r_read (STDIN_FILENO, buf_in, arg_in->bufsz);
      if (rd == 0) return /*0*/ -1;
      error_check (rd < 0) return -2;
      TODO (keep track of amount read)
      error_check (tscpaq_enqueue (&(arg_in->q_out), buf_in) != 0)
         return -3;
   /*}*/
   return 0;
#endif
}

__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result))
static int io_thread_cb_wr (
   size_t *restrict ret,
   io_thread_cb_t *restrict arg_out) {
   return io_thread_cb_common (
      ret,
      &(arg_out->q_out), &(arg_out->q_in), /*arg_out->bufsz*/ *ret,
      r_write, STDOUT_FILENO);
#ifdef OTHER
   char *restrict buf_out;
   ssize_t wr;
   /*while (true) {*/
      /* writer */
      error_check (tscpaq_dequeue (
         &(arg_out->q_out), (void const *restrict *restrict) &buf_out) != 0)
         return -1;
      wr = r_write (STDOUT_FILENO, buf_out, arg_out->bufsz);
      if (wr == 0) return -4;
      error_check (wr < 0) return -2;
      TODO (compare wr against amount read)
      error_check (wr != arg_out->bufsz) return -5;
      error_check (tscpaq_enqueue (&(arg_out->q_in),   buf_out) != 0)
         return -3;
   /*}*/
   return 0;
#endif
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *io_thread_cb (void *_arg) {
   io_thread_cb2_t *restrict arg = (io_thread_cb2_t *restrict) _arg;
   io_thread_cb_t *restrict arg_in;
   io_thread_cb_t *restrict arg_out;
   size_t tmp, sv;
   arg_in  = &(arg->in);
   arg_out = &(arg->out);
   while (true) {
      error_check (io_thread_cb_rd (&tmp, (void *) arg_in)  != 0) return NULL;
      if (tmp == 0) return NULL;
      sv = tmp;
      error_check (io_thread_cb_wr (&tmp, (void *) arg_out) != 0) return NULL;
      error_check (tmp != sv) return NULL;
   }
   return NULL;
}

#ifndef min
#define min(A, B) ((A) < (B) ? (A) : (B))
#endif

__attribute__ ((nothrow))
int main (void) {
   /*size_t bufsz = 512 * sizeof (char);
   size_t nbuf  = 3;
   char *restrict bufs;
   tscpaq_t q_in, q_out;*/
   io_thread_cb_t *restrict args_in;
   io_thread_cb_t *restrict args_out;
   io_thread_cb2_t args;
   pthread_t io_thread;
/*puts ("a"); fflush (stdout);*/
   args_in  = &(args.in);
   args_out = &(args.out);
/*puts ("b"); fflush (stdout);*/
   error_check (init_io_thread_cb (args_in,  (size_t)   3, (size_t) 3) != 0) return EXIT_FAILURE;
   error_check (init_io_thread_cb (args_out, (size_t)   3, (size_t) 3) != 0) return EXIT_FAILURE;
/*puts ("c"); fflush (stdout);*/
   pthread_create (&io_thread, NULL, io_thread_cb, (void *) &args);
/*puts ("d0"); fflush (stdout);*/
   while (true) { /* while other thread is alive*/
      char const *restrict buf_in;
      char *restrict buf_out;
/*puts ("e0"); fflush (stdout);*/
      error_check (tscpaq_dequeue (&(args_in->q_out), (void const *restrict *restrict) &buf_in)   != 0) {
         TODO (kill other thread);
         break;
      }
/*puts ("e00"); fflush (stdout);*/
      error_check (tscpaq_dequeue (&(args_out->q_in), (void const *restrict *restrict) &buf_out)  != 0) {
         TODO (kill other thread);
         break;
      }
      TODO (something else)
/*puts ("f0"); fflush (stdout);*/
      memcpy (buf_out, buf_in, min (args_in->bufsz, args_out->bufsz));
/*puts ("g0"); fflush (stdout);*/
      error_check (tscpaq_enqueue (&(args_out->q_out), buf_out) != 0) {
         TODO (kill other thread);
         break;
      }
/*puts ("g00"); fflush (stdout);*/
      error_check (tscpaq_enqueue (&(args_in->q_in),   buf_in)  != 0) {
         TODO (kill other thread);
         break;
      }
/*puts ("h0"); fflush (stdout);*/
   }
   /*__builtin_unreachable ();*/
/*puts ("i0"); fflush (stdout);*/

   TODO (pthread join)
   error_check (free_io_thread_cb (args_out) != 0) return EXIT_FAILURE;
   error_check (free_io_thread_cb (args_in)  != 0) return EXIT_FAILURE;
   return EXIT_SUCCESS;
   /*return EXIT_FAILURE;*/
}

