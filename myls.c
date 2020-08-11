#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <dirent.h>
#include "check_functions.h"
#include "print_info.h"
#include "sort_function.h"

extern int optind, opterr, optopt;
static int non_opts = 0;

int main (int argc, char *argv[]) {

    int count = 0;
    char *path = (char*)malloc(PATH_MAX);
    char cwd[PATH_MAX];
    Option *option = malloc(sizeof(Option));
    get_options(argc, argv, option); 

     for (int i = optind; i < argc; i++)
    {
        non_opts++;
    }

    //https://stackoverflow.com/questions/29158659/string-sort-in-c-using-array-of-pointers
    char* temp;
    int dif;
    for (int i = argc - non_opts; i< argc; i++) {
        for(int j = i + 1; j< argc; j++) {
            dif = myStringCmp(argv[i], argv[j]);
            if(dif > 0) {
                temp = argv[i];
                argv[i] = argv[j];
                argv[j] = temp;
            }
        }
    }
    // printf("Count the number of options: %d\n", count_opts);
    // printf("THE NUMBER OF NON ARGUMENTS %d\n", non_opts);

    // loop through all args and skips the options[ -i, -l, -il etc...]
    for (int i = argc - non_opts; i < argc; i++)
    { 
        if (!valid_directory(argv[i]) && !valid_file(argv[i])) {
            printf("ls: cannot access %s", argv[i]);
            printf("No such file or directory\n");
            exit(1);
        }

        // The number of args that are either a dir or a file
        else if (directory(argv[i]) || single_file(argv[i]) || valid_link(argv[i])) 
        {
            if (valid_directory(argv[i]) || valid_file(argv[i])) 
            {
                count++;
            }
        }
    }
    // printf("count %d\n",count);
    // https://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html
    // no options given
    if (count == 0) {
        // getcwd - get the pathname of the current working directory   
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            printf(" get cwd error %d\n", errno); 
            exit(1);
        }
        if(option->option_R) {
            recursive_print(".", option);
        } else {
            
            print_directory(cwd, option);
        }    
    }

    // [ARGUEMENTS]
    for (int j = argc - non_opts; j < argc; j++) 
    {   
       strcpy(path, argv[j]);   
        if (single_file(path) || valid_link(path))
        {
          
            print_file(path, option);
        } else if(directory(path)) {
            if(option->option_R) 
            {
                recursive_print(path, option);
            } 
            else
            {  
                printf("%s:\n", path);
                print_directory(path, option);
            }
        }
            
    }
    free(path);
    free(option);
    printf("\n");
    return 0;
}