#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tscpaq.h>

#define N (4)

int main(void) {
#ifdef TEST
   int arr[] = {101, 202, 303, 404, 505};
   int const *tmp;

   tscpaq_t q;
   if (tscpaq_alloc_queue (&q, (size_t) N) != 0) return EXIT_FAILURE;

   if (tscpaq_dumpq(&q, 1) != 0) return EXIT_FAILURE;
   if (tscpaq_enqueue (&q, arr + 0) != 0) return EXIT_FAILURE; if (tscpaq_dumpq(&q, 2) != 0) return EXIT_FAILURE;
   if (tscpaq_enqueue (&q, arr + 1) != 0) return EXIT_FAILURE; if (tscpaq_dumpq(&q, 3) != 0) return EXIT_FAILURE;
   if (tscpaq_enqueue (&q, arr + 2) != 0) return EXIT_FAILURE; if (tscpaq_dumpq(&q, 4) != 0) return EXIT_FAILURE;
   /*tscpaq_enqueue (&q, arr + 3);tscpaq_dumpq(&q, 5);
   tscpaq_enqueue (&q, arr + 4);tscpaq_dumpq(&q, 6);*/
   /*memset (arr, 0, sizeof (arr));*/
   if (tscpaq_dequeue (&q, (void const *restrict *)&tmp) != 0) return EXIT_FAILURE; printf ("%i\n", *tmp); if (tscpaq_dumpq(&q, 7) != 0) return EXIT_FAILURE;
   if (tscpaq_dequeue (&q, (void const *restrict *)&tmp) != 0) return EXIT_FAILURE; printf ("%i\n", *tmp); if (tscpaq_dumpq(&q, 8) != 0) return EXIT_FAILURE;
   if (tscpaq_dequeue (&q, (void const *restrict *)&tmp) != 0) return EXIT_FAILURE; printf ("%i\n", *tmp); if (tscpaq_dumpq(&q, 9) != 0) return EXIT_FAILURE;
   /*tscpaq_dequeue (&q, (void**)&tmp);
   if (tmp == NULL) puts ("NULL");
   else printf("%i\n", *tmp);
   tscpaq_dumpq(&q, 10);
   tscpaq_dequeue (&q, (void**)&tmp);
   if (tmp == NULL) puts ("NULL");
   else printf("%i\n", *tmp);
   tscpaq_dumpq(&q, 11);*/

   if (tscpaq_free_queue (&q) != 0) return EXIT_FAILURE;
#endif
   return EXIT_SUCCESS;
}