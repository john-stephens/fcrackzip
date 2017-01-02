
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_GETTIMEOFDAY
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#endif

#include "crack.h"
#include "benchmark.h"
#include "brutegen.h"
#include "callbacks.h"

static int benchmark_count;

int benchmark_gen (void) {
  if (!--benchmark_count) {
    return 0;
  }

  return brute_force_gen ();
}

void benchmark (void) {
#ifdef HAVE_GETTIMEOFDAY
  int i;
  long j, k;
  struct timeval tv1, tv2;
  method *crack_method = methods;
  int method_number = -1;

  do
    {
      for (i = 0; i < HEADER_SIZE * 3; i++)
        files[i] = i ^ (i * 3);

      file_count = 3;
      strcpy (pw, "abcdefghij");
      parse_charset ("a");
      benchmark_count = BENCHMARK_LOOPS;

      verbosity = 0;

      printf ("%c%s: ",
              (crack_method - methods == default_method) ? '*' : ' ',
              crack_method->desc);

      if (strncmp ("zip", crack_method->desc, 3))
        printf ("(skipped)");
      else
        {
          fflush (stdout);

          crack_method->init_crack_pw ();
          gettimeofday (&tv1, 0);
          crack_method->crack_pw (benchmark_gen, false_callback);
          gettimeofday (&tv2, 0);
          tv2.tv_sec -= tv1.tv_sec;
          tv2.tv_usec -= tv1.tv_usec;

          j = tv2.tv_sec * 1000000 + tv2.tv_usec;
          k = BENCHMARK_LOOPS;

          printf ("cracks/s = ");

          for (i = 7; i--;)
            printf ("%ld", k / j), k = (k - k / j * j) * 10;
        }

      printf ("\n");
      crack_method++;
    }
  while (method_number < 0 && crack_method->desc);
#else
  fprintf (stderr, "This executable was compiled without support for benchmarking\n");
  exit (1);
#endif
}

