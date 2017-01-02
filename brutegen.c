
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "crack.h"
#include "brutegen.h"

static int bf_min_length = -1;
static int bf_max_length = -1;

void parse_charset (char *cs) {
  u8 chars[800];
  u8 map[256];
  u8 *p = chars;

  while (*cs)
    switch (*cs++)
      {
      case 'a':
        strcpy ((char *) p, "abcdefghijklmnopqrstuvwxyz");
        p += 26;
        break;

      case 'A':
        strcpy ((char *) p, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        p += 26;
        break;

      case '1':
        strcpy ((char *) p, "0123456789");
        p += 10;
        break;

      case '!':
        strcpy ((char *) p, "!:$%&/()=?{[]}+-*~#");
        p += 18;
        break;

      case ':':
        while (*cs)
          *p++ = *cs++;
        break;

      default:
        fprintf (stderr, "unknown charset specifier, only 'aA1!:' recognized\n");
        exit (1);
      }

  *p = 0;

  p = chars;
  bf_last = *p++;
  memset (bf_next, bf_last, sizeof bf_next);
  memset (map, 0, 256);

  for (; *p; p++)
    if (!map[*p])
      {
        map[*p] = 1;
        bf_next[bf_last] = *p;
        bf_last = *p;
      }

  bf_next[bf_last] = chars[0];

/*  { int i; for (i = 0; i < 255; i++) printf ("bf_next [%3d] = %3d\n", i, bf_next[i]);}; */
}

void set_brute_force_length ( int min_length, int max_length ) {

  bf_min_length = min_length;
  bf_max_length = max_length;

  u8 *p = pw;
  while (p < pw + min_length) {
    *p++ = bf_next[255];
  }

  *p++ = 0;

}

int brute_force_gen (void) {
  u8 *p = pw_end;

  do {
    u8 o = *--p;
    *p = bf_next[o];
    if (o != bf_last) {
      return pw_end - p;
    }
  } while (p > pw);

  if (pw_end - pw < bf_max_length) {
    p = ++pw_end;
    *p = 0;

    while (p > pw) {
      *--p = bf_next[255];
    }

    return -1;
  } else {
    return 0;
  }
}

