
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "crack.h"
#include "brutegen.h"

static u8 bf_pw[MAX_PW+1];  // the brute force password
static u8 *bf_pw_end;       // pointer to the end of the password

static u8 bf_next[256];     // linked list of chars
static u8 bf_last;          // the last char that was checked

static int bf_min_length = -1;
static int bf_max_length = -1;

static int bf_first_pw = 0;

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

  *p = 0;               // null terminate chars
  p = chars;            // set p to start of chars
  bf_last = *p++;       // set bf_last to the first char to check and move p to the next one
  memset (bf_next, bf_last, sizeof bf_next); // fill bf_next with first char
  memset (map, 0, 256); // fill map with zeros

  for (; *p; p++) {     // iterate over all of the chars to check
    if (!map[*p]) {     // make sure we haven't seen this char before
      map[*p] = 1;      // note that we've seen this char
      bf_next[bf_last] = *p; // link the previous char to this char (essentially building a linked list)
      bf_last = *p;     // set bf_last to this char
    }
  }

  bf_next[bf_last] = chars[0]; // link the last char back to the first one

/*  { int i; for (i = 0; i < 255; i++) printf ("bf_next [%3d] = %3d\n", i, bf_next[i]);}; */
}

void set_brute_force_length (int min_length, int max_length) {

  bf_min_length = min_length;
  bf_max_length = max_length;

  u8 *p = bf_pw;    // pointer to the start of the password

  while (p < bf_pw + bf_min_length) {
    *p++ = bf_next[255]; // fill with the first character to the min length
  }

  *p++ = 0;         // null terminate the password
  bf_pw_end = bf_pw + bf_min_length;
  bf_first_pw = 1;

}

void set_brute_force_pw (const char *init_pw) {
  strcpy (bf_pw, init_pw);
  bf_pw_end = bf_pw + strlen (bf_pw);
  bf_first_pw = 1;
}

int brute_force_gen (u8 *pw_copy) {

  u8 *p = bf_pw_end;        // grab pointer to password end

  if (bf_first_pw) {
    bf_first_pw = 0;
    strcpy (pw_copy, bf_pw);
    return bf_pw - bf_pw_end;  // return the negative length of the password
  }

  do {

    u8 o = *--p;         // move pointer back and grab the password char
    *p = bf_next[o];     // set the password char to the next char to check

    if (o != bf_last) {  // make sure we're not on the last char to check...
      strcpy (pw_copy, bf_pw);
      return bf_pw_end - p; // return the number of chars changed
    }

  } while (p > bf_pw);      // loop so we can increment previous chars if needed

  if (bf_pw_end - bf_pw < bf_max_length) { // if we are not at max length...

    p = ++bf_pw_end;        // increment end pointer and copy to working pointer
    *p = 0;              // null terminate the password

    while (p > bf_pw) {     // fill the whole password with the first char to check
      *--p = bf_next[255];
    }

    strcpy (pw_copy, bf_pw);

    return bf_pw - bf_pw_end;  // return the negative length of the password

  } else {
    return 0;
  }

}

