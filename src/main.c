#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <threv.h>

__attribute__ ((nonnull (1, 2, 4), nothrow, warn_unused_result))
static int threv_cb (
   void *restrict dest,
   void const *restrict src,
   size_t srcsz, size_t *restrict destsz) {
   *destsz = min (srcsz, *destsz);
   memcpy (dest, src, *destsz);
   return 0;
}

__attribute__ ((nothrow, warn_unused_result))
int main (void) {
   error_check (threv (
      STDIN_FILENO, STDOUT_FILENO,
      (size_t) 10, (size_t) 3,
      (size_t) 8,  (size_t) 3,
      thrio_cb) != 0)
      return EXIT_FAILURE;
   return EXIT_SUCCESS;
}