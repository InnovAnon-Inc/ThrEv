#ifndef _TSCPAQ_H_
#define _TSCPAQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <pthread.h>
#include <semaphore.h>

#include <cpaq.h>
#include <glitter.h>

typedef struct {
	cpaq_t cpaq;
	pthread_mutex_t mutex;
	sem_t empty, full;
} tscpaq_t;

int tscpaq_init_queue (
	tscpaq_t *restrict q,
	void *arr[],
	size_t n)
__attribute__ ((leaf, nonnull (1, 2), nothrow, warn_unused_result)) ;

int tscpaq_uninit_queue (tscpaq_t *restrict q)
__attribute__ ((leaf, nonnull (1), nothrow, warn_unused_result)) ;

int tscpaq_alloc_queue (
	tscpaq_t *restrict q,
	size_t n)
__attribute__ ((nonnull (1), nothrow, warn_unused_result)) ;

int tscpaq_free_queue (tscpaq_t *restrict q)
__attribute__ ((nonnull (1), nothrow, warn_unused_result)) ;

/* ret type == elem type */
int tscpaq_dequeue (
	tscpaq_t *restrict q,
	void const *restrict *restrict ret)
__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result)) ;

/* returns err code */
int tscpaq_enqueue (
	tscpaq_t *restrict q,
	void *restrict elem)
__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result)) ;

int tscpaq_isempty (
	tscpaq_t *restrict q,
	bool *restrict ret)
__attribute__ ((leaf, nonnull (1, 2), nothrow, warn_unused_result)) ;

int tscpaq_isfull (
	tscpaq_t *restrict q,
	bool *restrict ret)
__attribute__ ((leaf, nonnull (1, 2), nothrow, warn_unused_result)) ;

int tscpaq_gethead (
	tscpaq_t *restrict q,
	void const *restrict *restrict ret)
__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result)) ;
/*
int tscpaq_dumpq(
	tscpaq_t *restrict q,
	int i)
__attribute__ ((nonnull (1), nothrow, warn_unused_result)) ;
*/
#ifdef __cplusplus
}
#endif

#endif /* _TSCPAQ_H_ */