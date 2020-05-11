AC_INIT([libtscpaq], [0.0], [laurence.a.maddox@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_AR
LT_INIT([])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])

AC_ARG_ENABLE([use-submodules],
    AS_HELP_STRING([--enable-use-submodules], [Enable use submodules]))
AM_CONDITIONAL([USE_SUBMODULES], [test x"$enable_use_submodules" = "xyes"])

AC_CONFIG_MACRO_DIR([m4])
#AC_CONFIG_SUBDIRS([glitter CPAQ])

AC_CHECK_HEADERS([stdio.h stdlib.h string.h sys/types.h])
AC_CHECK_HEADERS([pthread.h semaphore.h])
AC_CHECK_HEADERS([cpaq.h glitter.h])

AC_CHECK_LIB([pthread],[pthread_create,pthread_detach,pthread_kill])
AC_CHECK_LIB([pthread],[pthread_mutex_init,pthread_mutex_lock,pthread_mutex_unlock])
AC_CHECK_LIB([pthread],[pthread_cond_init,pthread_cond_wait,pthread_cond_signal,pthread_cond_broadcast])
AC_CHECK_LIB([pthread],[sem_init,sem_destroy,sem_wait,sem_post])
AC_CHECK_LIB([cpaq],[init_queue,free_queue,enqueue,dequeue,isempty,isfull,gethead,dumpq])
#AC_CHECK_LIB([cpaq],[alloc_queue])

INNOVANON_WARNINGS([])

AC_OUTPUT
