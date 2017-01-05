
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "crack.h"
#include "dictgen.h"

static u8 dict_pw[MAX_PW+1];

static FILE *dict_file;

int init_dictionary_gen (const char *filename) {

  if (!(dict_file = fopen (filename, "r"))) {
    return 0;
  } else {
    *dict_pw = 0;  // null terminate the password
    return 1;
  }

}

int dictionary_gen (u8 *pw_copy) {
  /* should optimize this, comparing prefixes would be a net win.
   * however, not using fgets but something better might be an
   * even higher win :(
   */
  int len = 0;

  if (fgets (dict_pw, MAX_PW+1, dict_file)) {
    len = strlen (dict_pw);
    dict_pw[len - 1] = 0;   // remove newline by reterminating
    strcpy (pw_copy, dict_pw);
    return 0 - len;
  } else {
    if (!feof (dict_file)) {
      perror ("dictionary_read_next_password");
    }

    return 0;
  }
}

int finish_dictionary_gen (void) {
  fclose (dict_file);
  return 1;
}
