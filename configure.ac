AC_INIT([libthrev], [0.0], [laurence.a.maddox@gmail.com])
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
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SUBDIRS([glitter TSCPAQ ThIpe libev])

AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([ev.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([glitter.h io.h tscpaq.h])

#AC_CHECK_LIB([stdlib],[])
AC_CHECK_LIB([ev],[ev_run,ev_io_init,ev_io_start,ev_io_stop,ev_break])
AC_CHECK_LIB([pthread],[pthread_create,pthread_join])
AC_CHECK_LIB([tscpaq],[tscpaq_enqueue,tscpaq_dequeue])
AC_CHECK_LIB([io],[alloc_io,free_io])
#AC_CHECK_LIB([restart],[r_close])
#AC_CHECK_LIB([ezudp-client],[ezudp_client])
#AC_CHECK_LIB([ezudp-server],[ezudp_server])
#AC_CHECK_LIB([thpool],[thpool_init,thpool_add_work,thpool_wait,thpool_pause,thpool_resume,thpool_destroy,thpool_num_threads_working])

INNOVANON_WARNINGS([])

AC_OUTPUT
