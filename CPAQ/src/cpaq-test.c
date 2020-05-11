#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <mmalloc.h>
#include <simon.h>

#include <cpaq.h>

__attribute__ ((nonnull (1), nothrow))
static void dumpq(cpaq_t const *restrict q) {
   size_t i;
   fputs ("Q: ", stderr);
   for (i = 0; i != used_space_cpaq (q); i++) {
      void *restrict head = *index_cpaq (q, i);
      fprintf (stderr, "(%1d:%3d), ", (int) i, *(int *restrict) head);
   }
   fputs ("\n", stderr);
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static void *cpaq_alloc (void const *restrict arg_) {
   size_t const *restrict arg = (size_t const *restrict) arg_;
   return ez_alloc_cpaq (*arg);
}

__attribute__ ((nonnull (1), nothrow))
static void degenerate_pint (void *restrict arg_) {
   int **restrict arg = (int **restrict) arg_;
#ifndef NDEBUG
   fprintf (stderr, "degenerate_pint (arg:%d)\n", **arg);
#endif
   free (*arg);
}

__attribute__ ((nonnull (1), nothrow))
static void degenerates_pint (void *restrict arg_, size_t n) {
   int **restrict arg = (int **restrict) arg_;
   size_t i;
   for (i = 0; i != n; i++) {
#ifndef NDEBUG
      fprintf (stderr, "degenerates_pint (n:%d)\n", (int) n);
      fprintf (stderr, "*(arg[i:%d]):%d\n",
         (int) i, *(arg[i]));
#endif
      free (arg[i]);
   }
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int generate_pint (void *restrict arg_) {
   int **restrict arg = (int **restrict) arg_;
   *arg = malloc (sizeof (int));
   error_check (*arg == NULL) return -1;
   **arg = random_range_java (-10, 10); /* arbitrary params */
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int generates_pint (void *restrict arg_, size_t n) {
   int **restrict arg = (int **restrict) arg_;
   size_t i;
   for (i = 0; i != n; i++)
      error_check (generate_pint (arg + i) != 0) {
         degenerates_pint (arg_, i);
         return -1;
      }
   return 0;
}

__attribute__ ((nonnull (1, 2), nothrow))
static void cpaq_enqueue (void *restrict ds, void const *restrict arg_) {
   cpaq_t *restrict cpaq = (cpaq_t *restrict) ds;
   void *const *restrict arg = (void *const *restrict) arg_;
#ifndef NDEBUG
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
   fprintf (stderr, "cpaq_enqueue (arg:%d)\n", **(int *const *restrict) arg);
	#pragma GCC diagnostic pop
#endif
   enqueue (cpaq, *arg);
#ifndef NDEBUG
   dumpq (cpaq);
#endif
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int cpaq_add_test (void *restrict arg_) {
   int *tmp;
   int err;
   fprintf (stderr, "cpaq_add_test ()\n");
   dumpq ((cpaq_t *restrict) arg_);
   err = padd_test (arg_, &tmp,
      (isfull_t) isfull, generate_pint, cpaq_enqueue);
   if (err == TEST_NA) return 0;
   error_check (err != 0) return -1;
   fprintf (stderr, "cpaq_add_test (), tmp:%d\n", *tmp);
   dumpq ((cpaq_t *restrict) arg_);
   return 0;
}

__attribute__ ((nonnull (1, 2), nothrow))
static void cpaq_dequeue (void *restrict ds, void *restrict arg_) {
   cpaq_t *restrict cpaq = (cpaq_t *restrict) ds;
   int const **restrict arg = (int const **restrict) arg_;
#ifndef NDEBUG
   fprintf (stderr, "cpaq_dequeue ()");
#endif
   *arg = dequeue (cpaq);
#ifndef NDEBUG
   fprintf (stderr, "**arg:%d\n", **arg);
   dumpq (cpaq);
#endif
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int cpaq_remove_test (void *restrict arg_) {
   int *tmp;
   int err;
   fprintf (stderr, "cpaq_remove_test ()\n");
   dumpq ((cpaq_t *restrict) arg_);
   err = premove_test (arg_, &tmp,
      (isempty_t) isempty, cpaq_dequeue, degenerate_pint);
   if (err == TEST_NA) return 0;
   error_check (err != 0) return -1;
   /*fprintf (stderr, "cpaq_remove_test (), tmp:%d\n", *tmp);*/
   dumpq ((cpaq_t *restrict) arg_);
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int cpaq_adds_test (void *restrict arg_) {
   int *tmps[13]; /* arbitrary params */
   fprintf (stderr, "cpaq_adds_test ()\n");
   dumpq ((cpaq_t *restrict) arg_);
   error_check (padds_test (arg_, tmps, ARRSZ (tmps),
      (remaining_space_t) remaining_space_cpaq,
      generates_pint, (adds_t) enqueues) != 0)
      return -1;
   /* can't print tmps, because we don't know how many elements are init'd */
   dumpq ((cpaq_t *restrict) arg_);
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int cpaq_removes_test (void *restrict arg_) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	#pragma GCC diagnostic ignored "-Wtrampolines"
   __attribute__ ((nonnull (1), nothrow))
   void degenerates_pint2 (void *restrict arg__, size_t n) {
	#pragma GCC diagnostic pop
      int **restrict arg = (int **restrict) arg__;
      size_t i;
      dumpq ((cpaq_t *restrict) arg_);
      for (i = 0; i != n; i++) {
   #ifndef NDEBUG
         fprintf (stderr, "degenerates_pint (n:%d)\n", (int) n);
         fprintf (stderr, "*(arg[i:%d]):%d\n",
            (int) i, *(arg[i]));
   #endif
         free (arg[i]);
      }
   }




   int *tmps[12]; /* arbitrary params */
   fprintf (stderr, "cpaq_removes_test ()\n");
   dumpq ((cpaq_t *restrict) arg_);
   error_check (premoves_test (arg_, tmps, ARRSZ (tmps),
      (used_space_t) used_space_cpaq, (removes_t) dequeues,
      (frees_t) degenerates_pint2) != 0)
   /*error_check (removes_test (arg_, tmps, ARRSZ (tmps),
      (used_space_t) used_space_cpaq, (removes_t) dequeues) != 0)
      return -1;*/
   /* can't print tmps, because we don't know how many elements are init'd */
   dumpq ((cpaq_t *restrict) arg_);
   return 0;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
static int cpaq_cb (void *restrict arg) {
   stdcb_t tests[4];

   TODO (more tests)
   tests[0] = cpaq_add_test;
   tests[1] = cpaq_remove_test;
   tests[2] = cpaq_adds_test;
   tests[3] = cpaq_removes_test;

   error_check (random_ops (arg, tests, ARRSZ (tests), 1000) != 0) /* arbitrary params */
   /*random_ops2 (arg, tests, ARRSZ (tests));*/
      return -1;
   frees_cpaq ((cpaq_t *restrict) arg, free);

   return 0;
}

int main(void) {
   time_t t;
   size_t n = 10; /* arbitrary params */

   t = time (NULL);
   srand ((unsigned int) t);

   error_check (ezmalloc (cpaq_alloc, &n,
      cpaq_cb,
      (free_t) ez_free_cpaq) != 0)
      return EXIT_FAILURE;

   return EXIT_SUCCESS;
}
