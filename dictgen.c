
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "crack.h"
#include "dictgen.h"

static FILE *dict_file;

int init_dictionary_gen (const char *filename) {

  if (!(dict_file = fopen (filename, "r"))) {
    return 0;
  } else {
    *(pw_end = pw) = 0;
    dictionary_gen (); /* fetch first password */
    return 1;
  }

}

int dictionary_gen (void) {
  /* should optimize this, comparing prefixes would be a net win.
   * however, not using fgets but something better might be an
   * even higher win :(
   */
  int len = 0;

  if (fgets (pw, MAX_PW+1, dict_file)) {
    len = strlen (pw);
    pw[len - 1] = 0;
    pw_end = pw + len;
    return pw - pw_end;
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
