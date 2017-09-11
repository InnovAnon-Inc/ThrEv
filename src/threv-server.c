#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h> /* for puts */
#include <stdlib.h>
#include <netinet/in.h>

#include <ev.h>

/*#include <restart.h>*/
#include <ezudp-server.h>
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
