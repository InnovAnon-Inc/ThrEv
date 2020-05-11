#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#include <stdlib.h>

#include <buffer.h>

__attribute__ ((leaf, nonnull (1, 2), nothrow))
void init_buffer (
   buffer_t *restrict buffer,
   char *restrict buf) {
   /*buffer->n = 0;*/
   buffer->buf = buf;
}

__attribute__ ((nonnull (1), nothrow, warn_unused_result))
int alloc_buffer (
   buffer_t *restrict buffer,
   size_t bufsz) {
   size_t i;
   char *restrict buf = malloc (bufsz);
   error_check (buf == NULL) return -1;
   init_buffer (buffer, buf);
   return 0;
}

__attribute__ ((leaf, nonnull (1), nothrow))
void free_buffer (buffer_t const *restrict buffer) {
   free (buffer->buf);
}
