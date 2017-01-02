
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "callbacks.h"
#include "crack.h"

int false_callback (const char *pw, const char *info) {
  (void) pw;
  (void) info;                        /* suppress warning */
  return 0;
}

int true_callback (const char *pw, const char *info) {
  (void) pw;
  (void) info;                        /* suppress warning */
  return 1;
}

int print_callback (const char *pw, const char *info) {
  if (!use_unzip || check_unzip (pw))
    {
      printf ("possible pw found: %s (%s)\n", pw, info ? info : "");
      /*exit(0); */
    }

  return 0;
}

