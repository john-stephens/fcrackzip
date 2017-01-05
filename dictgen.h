#ifndef DICTGEN_H
#define DICTGEN_H

int init_dictionary_gen (const char *filename);
int dictionary_gen (u8 *pw_copy);
int finish_dictionary_gen (void);

#endif
