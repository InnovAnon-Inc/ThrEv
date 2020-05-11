#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*#include <math.h>*/
#include <swap.h>

#include <simon.h>

TODO (struct std with RNG + seed etc)

__attribute__ ((const, leaf, nothrow, warn_unused_result))
unsigned int range_int (int min, int max) {
	int ret;
	assert (min <= max);
	ret = max - min + 1;
	assert (ret >= 0);
	return (unsigned int) ret;
}

/* https://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range */

__attribute__ ((leaf, nothrow, warn_unused_result))
int random_range_off_naive1 (unsigned int r, int off) {
	int ret;
	assert (r != 0);
	assert (r <= INT_MAX);
	/*assert (r <= INT_MAX - off); assert (r + off <= INT_MAX)*/
	TODO (allow other RNGs)
	ret = off + (rand () % (int) r);
	assert (off <= ret);
	assert (ret <= off + (int) r);
	return ret;
}

/* probabilities are slightly skewed */
__attribute__ ((nothrow, warn_unused_result))
int random_range_naive1 (int min, int max) {
	TODO (allow other RNGs)
	int ret = random_range_off_naive1 (range_int (min, max), min);
	assert (min <= ret);
	assert (ret <= max);
	return ret;
}

/* the "skew" is a bit better distributed here */
__attribute__ ((leaf, nothrow, warn_unused_result))
int random_range_naive2 (int min, int max) {
	unsigned int n = range_int (min, max);
	int tmp;
	double ret;
	assert (n != 0);
	tmp = rand ();
	ret = (double) tmp / (double) RAND_MAX * (double) n;
	tmp = (int) ret;
	assert (min <= tmp);
	assert (tmp <= max);
	return tmp;
}

/* warning: there's some sort of effect due to the interaction of this algo
 * and some pRNGs */
__attribute__ ((leaf, nothrow, warn_unused_result))
int random_range_off_java (unsigned int n, int off) {
	int r;
	int d;
	int x;
	assert (n != 0);
	assert (n <= INT_MAX);
	assert (n != 1);
	r = RAND_MAX % (int) n;
	d = RAND_MAX - (int) r;
	do x = rand ();
	while (x >= d);
	/*assert (0 <= x % (int) n);
	assert (x % (int) n <= n);*/
	r = off + x % (int) n;
	assert (off <= r);
	assert (r <= off + (int) n);
	return r;
}

__attribute__ ((const, leaf, nothrow, warn_unused_result))
int range_int2 (int min, int max) {
	int ret;
	assert (min <= max);
	ret = max - min + 1;
	assert (ret >= 0);
	return ret;
}

__attribute__ ((leaf, nothrow, warn_unused_result))
int random_range_java2 (int min, int max) {
	int r = range_int2 (min, max);
	int ret;
	assert (r >= 1);
	assert (r == max - min + 1);
	ret = random_range_off_java2 ((unsigned int) r, min);
	assert (min <= ret);
	assert (ret <= max);
	return ret;
}

__attribute__ ((leaf, nothrow, warn_unused_result))
int random_range_off_java2 (unsigned int n, int off) {
	int r = RAND_MAX % (int) n;
	int d = RAND_MAX - (int) r;
	int x;
	assert (n > 1);
	do x = rand ();
	while (x >= d);
	r = off + x % (int) n;
	return r;
}













__attribute__ ((nothrow, warn_unused_result))
int random_range_java (int min, int max) {
	unsigned int r = range_int (min, max);
	int ret;
	assert ((int) r == max - min + 1);
	ret = random_range_off_java (r, min);
	assert (min <= ret);
	assert (ret <= max);
	return ret;
}

__attribute__ ((nonnull (1, 3), nothrow))
void random_ranges (int dest[], size_t n,
	random_range_t cb, int min, int max) {
	size_t k;
	for (k = 0; k != n; k++)
		dest[k] = cb (min, max);
}

__attribute__ ((nonnull (1, 3), nothrow))
void random_ranges_uniq_knuth (int dest[], size_t n,
	random_range_t cb, int min, int max, int tmp[]) {
	size_t k;
	for (k = 0; k != n; k++)
		dest[k] = cb (0, max - min);
	#pragma GCC ivdep
	for (k = 0; k != n; k++)
		dest[k] += min;
}

__attribute__ ((nonnull (1), nothrow))
void ez_random_ranges (int dest[], size_t n,
	int min, int max) {
	random_ranges (dest, n, random_range_java, min, max);
}

__attribute__ ((const, leaf, nothrow, warn_unused_result))
ssize_t range_ssize_t (ssize_t min, ssize_t max) {
	return max - min + 1;
}

__attribute__ ((const, leaf, nothrow, warn_unused_result))
size_t range_size_t (size_t min, size_t max) {
	return max - min + 1;
}

TODO (this algorihtm is probably seriously flawed)
__attribute__ ((nothrow, warn_unused_result))
size_t random_range_java_size_t (size_t min, size_t max) {
	size_t n = range_size_t (min, max);
	size_t r = n % (size_t) RAND_MAX;
	int tmp  = abs ((int) ((size_t) RAND_MAX - r));
	size_t d = (size_t) tmp;
	/* assumes sizeof (size_t) % sizeof (char) == 0 */
	char x[sizeof (size_t) / sizeof (char)];
	do ez_random_ranges ((int *restrict) x, sizeof (size_t), 0, CHAR_MAX);
	while (*(size_t *) x >= d);
	return min + *(size_t *) x % n;
}

__attribute__ ((nothrow, warn_unused_result))
size_t random_range_java_size_t2 (size_t min, size_t max) {
	int tmp;
	size_t ret;
	assert (min <= max);
	assert (max <= INT_MAX);
	tmp = random_range_java2 ((int) min, (int) max);
	assert (tmp >= 0);
	ret = (size_t) tmp;
	assert (min <= ret);
	assert (ret <= max);
	return ret;
}

__attribute__ ((nothrow, warn_unused_result))
ssize_t random_range_java_ssize_t2 (ssize_t min, ssize_t max) {
	int tmp;
	ssize_t ret;
	assert (min <= max);
	assert (max <= INT_MAX);
	tmp = random_range_java ((int) min, (int) max);
	ret = (ssize_t) tmp;
	assert (min <= ret);
	assert (ret <= max);
	return ret;
}

__attribute__ ((nonnull (1, 3, 4, 5), nothrow))
void random_range_naive_generic (void *restrict dest, size_t esz,
	cmp_t cmp, void const *restrict min, void const *restrict max) {
	do ez_random_ranges (dest, esz, 0, CHAR_MAX);
	while (cmp (dest, min) < 0 || cmp (dest, max) > 0) ;
}





/*
__attribute__ ((leaf, nonnull (1, 2, 3), nothrow, warn_unused_result))
int init_struct (init_struct_field_t const cbs[],
	void *restrict const fields[],
	void const *restrict const args[], size_t ncb) {
	size_t i;
	for (i = 0; i != ncb; i++)
		error_check (cbs[i] (fields[i], args[i]) != 0)
			return -1;
	return 0;
}
*/
/*
void *restrict random_element () {

}
*/
/*
int random_operation (void *restrict ds, stdcb_t cbs[], void *restrict args[], size_t ncb) {
	size_t i = random_range_java_size_t (0, ncb - 1);
	return cbs[i] (ds, args[i]);
}
*/
/*
int random_operation (void *restrict ds, stdcb_t cbs[], void *restrict args[], size_t ncb) {
	size_t i = random_range_java_size_t (0, ncb - 1);
	return cbs[i] (ds, args[i]);
}
*/

__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result))
int random_op (void *restrict ds, stdcb_t const tests[], size_t ntest) {
   size_t j = random_range_java_size_t2 ((size_t) 0, ntest - 1);
   int res;
   assert (j <= ntest);
   res = tests[j] (ds);
   if (res == TEST_NA) return TEST_NA;
   error_check (res != 0) return -1;
   return 0;
}

__attribute__ ((nonnull (1, 2), nothrow, warn_unused_result))
int random_ops (void *restrict ds, stdcb_t const tests[], size_t ntest,
	unsigned int n) {
	unsigned int i;
	int res;
	for (i = 0; i != n; ) {
		res = random_op (ds, tests, ntest);
		if (res == TEST_NA) continue;
		error_check (res != 0) return -1;
		i++;
	}
	return 0;
}

__attribute__ ((nonnull (1, 2), nothrow))
void random_ops2 (void *restrict ds, stdcb_t const tests[], size_t ntest) {
	int res;
	while (true) {
		res = random_op (ds, tests, ntest);
		/*if (res == TEST_NA) continue;*/
		error_check (res != 0 && res != TEST_NA) return /*-1*/;
	}
	assert (false);
	__builtin_unreachable ();
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 3, 4, 5), nothrow, warn_unused_result))
int add_test (void *restrict arg, void *restrict tmp,
   isfull_t full, generate_t generate, add_t add) {
   if (full (arg)) return TEST_NA;
   generate (tmp);
   add (arg, tmp);
   /*
   tmp = random_range_java (-10, 10);
   enqueue (arg, &tmp);
   dumpq (arg);
   */
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 3, 4), nothrow, warn_unused_result))
int remove_test (void *restrict arg, void *restrict tmp,
   isempty_t empty, remove_t remove) {
   if (empty (arg)) return TEST_NA;
   remove (arg, tmp);
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 4, 5, 6), nothrow, warn_unused_result))
int adds_test (void *restrict arg, void *restrict tmp, size_t maxn,
   remaining_space_t remaining, generates_t generates, adds_t adds) {
   /*size_t i;*/
   size_t n = min (maxn, remaining (arg));
   if (n != 0)
      n = random_range_java_size_t2 ((size_t) 0, n);
#ifdef TEST
   /*#pragma GCC ivdep*/
   for (i = 0; i != n; i++)
      /* either use array or parray */
      generate (tmp + ?);
#endif
   generates (tmp, n);
   adds (arg, tmp, n);
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 4, 5), nothrow, warn_unused_result))
int removes_test (void *restrict arg, void *restrict tmp, size_t maxn,
   used_space_t used, removes_t removes) {
   size_t n = min (maxn, used (arg));
   if (n != 0)
      n = random_range_java_size_t2 ((size_t) 0, n);
   removes (arg, tmp, n);
   return 0;
}






TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 3, 4, 5), nothrow, warn_unused_result))
int padd_test (void *restrict arg, void *restrict tmp,
   isfull_t full, pgenerate_t generate, add_t add) {
   if (full (arg)) return TEST_NA;
   error_check (generate (tmp) != 0) return -1;
   add (arg, tmp);
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 4, 5, 6), nothrow, warn_unused_result))
int padds_test (void *restrict arg, void *restrict tmp, size_t maxn,
   remaining_space_t remaining, pgenerates_t generates, adds_t adds) {
   /*size_t i;*/
   size_t n = min (maxn, remaining (arg));
   if (n != 0)
      n = random_range_java_size_t2 ((size_t) 0, n);
   error_check (generates (tmp, n) != 0) return -1;
   adds (arg, tmp, n);
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 3, 4, 5), nothrow, warn_unused_result))
int premove_test (void *restrict arg, void *restrict tmp,
   isempty_t empty, remove_t remove, free_t f) {
   if (empty (arg)) return TEST_NA;
   remove (arg, tmp);
   f (tmp);
   return 0;
}

TODO (add wrapper to check sizes)
__attribute__ ((nonnull (1, 2, 4, 5, 6), nothrow, warn_unused_result))
int premoves_test (void *restrict arg, void *restrict tmp, size_t maxn,
   used_space_t used, removes_t removes, frees_t f) {
   size_t i;
   size_t n = min (maxn, used (arg));
   if (n != 0)
      n = random_range_java_size_t2 ((size_t) 0, n);
   removes (arg, tmp, n);
   f (tmp, n);
   return 0;
}

TODO (insert test)
TODO (uninsert test)

/* init
 * vs
 * alloc + free
 */
/* insert + bulk
 * remove + bulk
 */

/*
typedef struct {
	int min, max;
	int res;
} generate_int_t;

__attribute__ ((nonnull (1), nothrow))
void generate_int (void *restrict arg_) {
   int *restrict arg = (int *restrict) arg_;
   *arg = random_range_java (-10, 10);
}
*/



TODO (print_cb instead of hardcoded type and format string)
TODO (snprintf)
/*
static void data_print (void const *restrict data,
   size_t i, size_t j) {
   fprintf (stderr, "["); fflush (stderr);
   / *if (array->n != 0) {* /
   if (i != j) {
      fprintf (stderr, "%d", ((int const *restrict) data)[i]); fflush (stderr);
      for (i++; i != j; i++)
         fprintf (stderr, ", %d", ((int const *restrict) data)[i]); fflush (stderr);
   }
   fprintf (stderr, "]\n"); fflush (stderr);
}
*/