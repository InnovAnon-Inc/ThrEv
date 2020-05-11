#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <restart.h>

/* http://pubs.opengroup.org/onlinepubs/007904975/basedefs/limits.h.html */
#ifndef _POSIX_PIPE_BUF
#define _POSIX_PIPE_BUF (512)
#endif
#ifndef PIPE_BUF
#define PIPE_BUF _POSIX_PIPE_BUF
#endif

#define BLKSIZE PIPE_BUF
#define MILLION (1000000L)
#define D_MILLION (1000000.0)

/* Private functions */

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
int gettimeout(struct timeval end,
               struct timeval *restrict timeoutp) {
   (void) gettimeofday(timeoutp, NULL);
   timeoutp->tv_sec = end.tv_sec - timeoutp->tv_sec;
   timeoutp->tv_usec = end.tv_usec - timeoutp->tv_usec;
   if (timeoutp->tv_usec >= MILLION) {
      timeoutp->tv_sec++;
      timeoutp->tv_usec -= MILLION;
   }
   if (timeoutp->tv_usec < 0) {
      timeoutp->tv_sec--;
      timeoutp->tv_usec += MILLION;
   }
   error_check ((timeoutp->tv_sec < 0) ||
       ((timeoutp->tv_sec == 0) && (timeoutp->tv_usec == 0))) {
      errno = ETIME;
      return -1;
   }
   return 0;
}

/* Restart versions of traditional functions */

__attribute__ ((leaf, nothrow, warn_unused_result))
int r_close(fd_t fildes) {
   int retval;
   do retval = close (fildes);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nothrow, warn_unused_result))
fd_t r_dup2(fd_t fildes, fd_t fildes2) {
   fd_t retval;
   do retval = dup2 (fildes, fildes2);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
fd_t r_open2(const char path[], int oflag) {
   fd_t retval;
   do retval = open (path, oflag);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
fd_t r_open3(const char path[], int oflag, mode_t mode) {
   fd_t retval;
   do retval = open (path, oflag, mode);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
ssize_t r_read(fd_t fd, void *restrict buf, size_t size) {
   ssize_t retval;
   do retval = read(fd, buf, size);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
pid_t r_wait(int *restrict stat_loc) {
   pid_t retval;
   do retval = wait (stat_loc);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
pid_t r_waitpid(pid_t pid, int *restrict stat_loc, int options) {
   pid_t retval;
   do retval = waitpid (pid, stat_loc, options);
   while_echeck (retval == -1, EINTR) ;
   return retval;
}

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
ssize_t r_write(fd_t fd, void *restrict buf, size_t size) {
   char *restrict bufp;
   size_t bytestowrite;
   ssize_t byteswritten;
   size_t totalbytes;

   TODO (verify that size <= SSIZE_MAX)

   for (bufp = buf, bytestowrite = size, totalbytes = 0;
        bytestowrite > 0;
        bufp += byteswritten, bytestowrite -= (size_t) byteswritten) {
      byteswritten = write(fd, bufp, bytestowrite);
      error_check ((byteswritten) == -1 && (errno != EINTR))
         return -1;
      error_check (byteswritten == -1) /* errno == EINTR */
         byteswritten = 0;
      totalbytes += (size_t) byteswritten;
   }
   return (ssize_t) totalbytes;
}

/* Utility functions */

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Waggregate-return"
__attribute__ ((leaf, nothrow, warn_unused_result))
struct timeval add2currenttime(double seconds) {
	#pragma GCC diagnostic pop
   struct timeval newtime;

   gettimeofday(&newtime, NULL);
   newtime.tv_sec += (int)seconds;
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunsuffixed-float-constants"
   newtime.tv_usec += (int)((seconds - (int)seconds)*D_MILLION + 0.5);
	#pragma GCC diagnostic pop
   if (newtime.tv_usec >= MILLION) {
      newtime.tv_sec++;
      newtime.tv_usec -= MILLION;
   }
   return newtime;
}

__attribute__ ((nothrow, warn_unused_result))
size_t copyfile(fd_t fromfd, fd_t tofd) {
   ssize_t bytesread;
   size_t totalbytes = 0;

   while ((bytesread = readwrite(fromfd, tofd)) > 0)
      totalbytes += (size_t) bytesread;
   return totalbytes;
}

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
ssize_t readblock(fd_t fd, void *restrict buf, size_t size) {
   char *restrict bufp;
   ssize_t bytesread;
   size_t bytestoread;
   size_t totalbytes;

   TODO (verify that size <= SSIZE_MAX)

   for (bufp = buf, bytestoread = size, totalbytes = 0;
        bytestoread > 0;
        bufp += bytesread, bytestoread -= (size_t) bytesread) {
      bytesread = read(fd, bufp, bytestoread);
      if ((bytesread == 0) && (totalbytes == 0))
         return 0;
      error_check (bytesread == 0) {
         errno = EINVAL;
         return -1;
      }
      error_check ((bytesread) == -1 && (errno != EINTR))
         return -1;
      error_check (bytesread == -1) /* errno == EINTR */
         bytesread = 0;
      totalbytes += (size_t) bytesread;
   }
   return (ssize_t) totalbytes;
}

__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result))
ssize_t readline(fd_t fd, char buf[], size_t nbytes) {
   size_t numread = 0;
   ssize_t returnval;

   TODO (verify that size <= SSIZE_MAX)

   while_expect (numread < nbytes - 1) { /* (see below) */
      returnval = read(fd, buf + numread, (size_t) 1);
      error_check ((returnval == -1) && (errno == EINTR))
         continue;
      if ((returnval == 0) && (numread == 0))
         return 0;
      if (returnval == 0)
         break;
      error_check (returnval == -1)
         return -1;
      numread++;
      if (buf[numread-1] == '\n') {
         buf[numread] = '\0';
         return (ssize_t) numread;
      }
   } /* expect true, because exiting loop like that is error-case */
   errno = EINVAL;
   return -1;
}

__attribute__ ((nonnull (2), nothrow, warn_unused_result))
ssize_t readtimed(
   fd_t fd,
   void *restrict buf, size_t nbyte,
   double seconds) {
   struct timeval timedone;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Waggregate-return"
   timedone = add2currenttime(seconds);
	#pragma GCC diagnostic pop
   error_check (waitfdtimed(fd, timedone) == -1)
      return (ssize_t)(-1);
   return r_read(fd, buf, nbyte);
}

__attribute__ ((nothrow, warn_unused_result))
ssize_t readwrite(fd_t fromfd, fd_t tofd) {
   char buf[BLKSIZE];
   ssize_t bytesread;

   error_check ((bytesread = r_read(fromfd, buf, (size_t) BLKSIZE)) < 0)
      return -1;
   if (bytesread == 0)
      return 0;
   error_check (r_write(tofd, buf, (size_t) bytesread) < 0)
      return -1;
   return bytesread;
}

__attribute__ ((nonnull (3), nothrow, warn_unused_result))
ssize_t readwriteblock(
   fd_t fromfd, fd_t tofd,
   char buf[], size_t size) {
   ssize_t bytesread;

   bytesread = readblock(fromfd, buf, size);
   error_check (bytesread != (ssize_t) size) /* can only be 0 or -1 */
      return bytesread;
   return r_write(tofd, buf, size);
}

TODO(is waitfdtimed() a leaf?)
__attribute__ ((nothrow, warn_unused_result))
int waitfdtimed(fd_t fd, struct timeval end) {
   fd_set readset;
   fd_t retval;
   struct timeval timeout;

   error_check ((fd < 0) || (fd >= FD_SETSIZE)) {
      errno = EINVAL;
      return -1;
   }
   FD_ZERO(&readset);
   TODO (ignoring error because Android refuses to cross compile otherwise)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-conversion"
   FD_SET(fd, &readset);
	#pragma GCC diagnostic pop
   error_check (gettimeout(end, &timeout) == -1)
      return -1;
   do {
      retval = select(fd+1, &readset, NULL, NULL, &timeout);
      error_check (retval == -1)
         if (errno == EINTR)
            continue;
   /*
   while_echeck (
      (retval = select(fd+1, &readset, NULL, NULL, &timeout)) == -1,
      EINTR) {
   */
      error_check (gettimeout(end, &timeout) == -1)
         return -1;
      FD_ZERO(&readset);
   TODO (ignoring error because Android refuses to cross compile otherwise)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-conversion"
      FD_SET(fd, &readset);
	#pragma GCC diagnostic pop
   } while (true);
   error_check (retval == 0) {
      errno = ETIME;
      return -1;
   }
   error_check (retval == -1)
      return -1;
   return 0;
}

/*
__attribute__ ((leaf, nothrow))
void r_sleep (unsigned int seconds) {
   unsigned int left = seconds;
   if (left == 0) return;
   do left = sleep (left);
   while_check (left != 0) ;
}
*/
__attribute__ ((nothrow, warn_unused_result))
int r_sleep (unsigned int seconds) {
	/*fd_set set;*/
	struct timeval timeout;
	/*int ret;*/

	/*FD_ZERO (&set);*/
	/*FD_SET (filedes, &set);*/

	timeout.tv_sec  = seconds;
	timeout.tv_usec = 0;

	/*do ret = select (FD_SETSIZE, &set, NULL, NULL, &timeout);
	while_echeck (ret == -1, EINTR);
	return ret;*/ /* 0 or -1 */

   return r_sleept (&timeout);
}

__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result))
int r_sleept (struct timeval *restrict timeout) {
	fd_set set;
	int ret;

	FD_ZERO (&set);
	/*FD_SET (filedes, &set);*/

	do ret = select (FD_SETSIZE, &set, NULL, NULL, timeout);
	while_echeck (ret == -1, EINTR);
	return ret; /* 0 or -1 */
}
