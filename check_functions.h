#ifndef _CHECK_FUNCTIONS_H_
#define _CHECK_FUNCTIONS_H_

typedef struct Options {
    int option_i;
    int option_l;
    int option_R;
} Option;

int valid_directory(char*);
int valid_file(char*); 
int single_file(char*); 
int directory(char*);
int valid_link(char*); 
void get_options(const int, char **argv, Option*); 


#endif