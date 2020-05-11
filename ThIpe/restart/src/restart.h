#ifndef _RESTART_H_
#define _RESTART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include <glitter.h>

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

int gettimeout(struct timeval end,
               struct timeval *restrict timeoutp)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Waggregate-return"
struct timeval add2currenttime(double seconds)
__attribute__ ((leaf, nothrow, warn_unused_result)) ;
	#pragma GCC diagnostic pop

size_t copyfile(fd_t fromfd, fd_t tofd)
__attribute__ ((nothrow, warn_unused_result)) ;

int r_close(fd_t fildes)
__attribute__ ((leaf, nothrow, warn_unused_result)) ;

fd_t r_dup2(fd_t fildes, fd_t fildes2)
__attribute__ ((leaf, nothrow, warn_unused_result)) ;

fd_t r_open2(const char path[], int oflag)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

fd_t r_open3(const char path[], int oflag, mode_t mode)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

ssize_t r_read(fd_t fd, void *restrict buf, size_t size)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

pid_t r_wait(int *restrict stat_loc)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

pid_t r_waitpid(pid_t pid, int *restrict stat_loc, int options)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

ssize_t r_write(fd_t fd, void *restrict buf, size_t size)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

ssize_t readblock(fd_t fd, void *restrict buf, size_t size)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

ssize_t readline(fd_t fd, char buf[], size_t nbytes)
__attribute__ ((leaf, nonnull (2), nothrow, warn_unused_result)) ;

ssize_t readtimed(fd_t fd, void *restrict buf, size_t nbyte, double seconds)
__attribute__ ((nonnull (2), nothrow, warn_unused_result)) ;

ssize_t readwrite(fd_t fromfd, fd_t tofd)
__attribute__ ((nothrow, warn_unused_result)) ;

ssize_t readwriteblock(fd_t fromfd, fd_t tofd, char buf[], size_t size)
__attribute__ ((nonnull (3), nothrow, warn_unused_result));

int waitfdtimed(fd_t fd, struct timeval end)
__attribute__ ((nothrow, warn_unused_result)) ;

int r_sleep (unsigned int seconds)
__attribute__ ((nothrow, warn_unused_result)) ;

int r_sleept (struct timeval *restrict seconds)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

#ifdef __cplusplus
}
#endif

#endif /* _RESTART_H_ */
