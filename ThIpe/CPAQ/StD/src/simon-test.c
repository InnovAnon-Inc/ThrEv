#if HAVE_CONFIG_H
#include <config.h>
#endif

#define _POSIX_C_SOURCE 200112L
#define __STDC_VERSION__ 200112L

#define NDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*#include <math.h>*/

#include <simon.h>

__attribute__ ((nothrow, warn_unused_result))
int main (void) {
   time_t t;
   int nums[100];
   int minn = 10;
   int maxn = 20;
   size_t ntest = 100;
   size_t testi;

   t = time (NULL);
   srand ((unsigned int) t);

   ez_random_ranges (nums, ARRSZ (nums), 10, 20);
   /*data_print (nums, (size_t) 0, ARRSZ (nums));*/

   fputs ("success", stderr); fflush (stderr);

   return EXIT_SUCCESS;
}
