#ifndef BRUTEGEN_H
#define BRUTEGEN_H

void parse_charset (char *cs);
void set_brute_force_length ( int min_length, int max_length );
//void set_brute_force_pw( const char *pw );
//void init_brute_force_gen( void );
int brute_force_gen (void);

#endif
