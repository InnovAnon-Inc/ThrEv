#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdio.h> /* for puts */
#include <stdlib.h>
#include <netinet/in.h>

#include <glitter.h>

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
	#pragma GCC diagnostic ignored "-Wnested-externs"
#include <ev.h>
	#pragma GCC diagnostic pop

/*#include <restart.h>*/
#include <ezudp-client.h>




	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   ev_io io;
   socket_t s;
} socket_rd_watcher_t;
	#pragma GCC diagnostic pop

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpadded"
typedef struct {
   ev_io io;
   struct ev_loop *loop;
   socket_t s;
   struct sockaddr_in *si_other;
} socket_wr_watcher_t;
	#pragma GCC diagnostic pop




static void
socket_rd_cb (EV_P_ ev_io *w_, int revents) {
   socket_rd_watcher_t *w = (socket_rd_watcher_t *) w_;
   struct sockaddr_in si_other;
   socklen_t slen = sizeof (si_other);

   char buf[1024];
   ssize_t recv_len = recvfrom (w->s, buf, sizeof (buf), 0, (struct sockaddr *) &si_other, &slen);
   puts ("test");
   if (recv_len == -1) {
      free (w);
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }
   buf[recv_len - 1] = '\0';
   puts (buf);

   free (w);
   ev_io_stop (EV_A_ &(w->io));
   ev_break (EV_A_ EVBREAK_ALL);
   return;
}

static void
socket_wr_cb (EV_P_ ev_io *w_, int revents) {
   socket_wr_watcher_t *w = (socket_wr_watcher_t *) w_;
   char buf[] = "Hello, World!";
   socklen_t slen = sizeof (*(w->si_other));

   socket_rd_watcher_t *rd_watcher = (socket_rd_watcher_t *) malloc (sizeof (socket_rd_watcher_t));
   if (rd_watcher == NULL) {
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }
   rd_watcher->s = w->s;

   if (sendto (w->s, buf, sizeof (buf), 0, (struct sockaddr *) w->si_other, slen) == -1) {
      free (rd_watcher);
      ev_io_stop (EV_A_ &(w->io));
      ev_break (EV_A_ EVBREAK_ALL);
      return;
   }

   ev_io_init (&(rd_watcher->io), socket_rd_cb, rd_watcher->s, EV_READ);
   ev_io_start (w->loop, (ev_io *) rd_watcher);

   ev_io_stop (EV_A_ &(w->io));
}











int ezudpcb (socket_t s, struct sockaddr_in *si_other, void *unused) {
   struct ev_loop *loop = EV_DEFAULT;
   socket_wr_watcher_t wr_watcher;

   wr_watcher.loop = loop;
   wr_watcher.s = s;
   wr_watcher.si_other = si_other;
   ev_io_init (&(wr_watcher.io), socket_wr_cb, s, EV_WRITE);
   ev_io_start (loop, (ev_io *) &wr_watcher);

   ev_run (loop, 0);

   return 0;
}

int main (void) {
   const int err = ezudp_client (1234, "127.0.0.1" /*"localhost"*/, ezudpcb, NULL);
   if (err != 0) {
      fprintf (stderr, "err:%d\n", err);
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
