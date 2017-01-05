
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "crack.h"
#include "validate.h"
#include "callbacks.h"

static u8 validate_pw[MAX_PW+1];
static int validate_pw_first = 1;

int validate_gen (u8 *pw_copy) {
  if (validate_pw_first == 0) {
    return 0;
  } else {
    validate_pw_first = 0;           // note that we've been through here once
    strcpy (pw_copy, validate_pw);   // copy the validate pw
    return 0 - strlen (validate_pw); // return the negative length of the pw
  }
}

void validate (method *crack_method) {
  u8 header[HEADER_SIZE + 1] =
  {0xf4, 0x28, 0xd6, 0xee, 0xd7, 0xd2,
   0x3c, 0x1a, 0x20, 0xab, 0xdf, 0x73,
   0xd6, 0xba, 0};                /* PW: "Martha" */
  strcpy ((char *) files, (char *) header);        /* yeah, dirty... */
  file_count = 1;

  if (crack_method->desc[0] == 'z') {
    crack_method->init_crack_pw ();

    strcpy (validate_pw, "Martha");
    if (crack_method->crack_pw (validate_gen, true_callback)) {
      printf ("validate ok (%s == Martha)\n", validate_pw);
    } else {
      printf ("validation error (%s != Martha)\n", validate_pw);
    }
  } else {
    printf ("validate only works for zip methods, use --method to select one.\n");
  }
}

