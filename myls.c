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

typedef struct Options {
    int option_i;
    int option_l;
    int option_R;
} Option;

void get_options(const int, char **argv, Option*); 
void get_filename(char*, struct stat, Option *); 
void print_file(char*,Option*); 
int print_directory(char*, Option*);

extern int optind, opterr, optopt;
static int non_opts = 0;

void get_options(const int argc, char **argv, Option* opt) {
    int choice;
    struct stat s_stat;

    while((choice = getopt(argc, argv, "+ilR")) != -1)
    {   
        switch(choice) {
            case 'i':  
                opt->option_i = 1;
                break;
            case 'l':  
                opt->option_l = 1;
                break;
            case 'R':  
                opt->option_R = 1;
                break;
            default:
                exit(1);  
            
        }
    }
    for (int i = optind; i < argc; i++)
    {
        non_opts++;
    }
}

void get_filename(char* name,struct stat s_stat, Option *opt) {
    struct stat temp_stat;
    int max_width = 512; 
    //check if is afile
    if (S_ISREG(s_stat.st_mode)) 
    {
        if (opt->option_R && !opt->option_l)
        {
            printf("%.*s ", max_width, name);
            
        }
        else
        {
            printf("%.*s\n", max_width, name);
        }
        return; 
    }

    // check if is a directory
    if (S_ISDIR(s_stat.st_mode)) 
    {
        if (opt->option_R && !opt->option_l) 
        {
            printf("%.*s ", max_width, name);
            
        }
        else
        {
            printf("%.*s\n", max_width, name);
        }
        return;
    }
    // check if is a link
    if (S_ISLNK(s_stat.st_mode))
    {
        printf("%.*s ", max_width, name);
    }
    
    // buffer contains the link of a filename
    char *buffer = (char*)malloc(PATH_MAX);
    size_t pathlen = readlink(name, buffer, sizeof(buffer) - 1);
    buffer[pathlen] = 0;
    lstat(buffer, &temp_stat);

    //if is a link, then we will need to print out the file/dir its poiting to.
    if (S_ISLNK(s_stat.st_mode) && opt->option_l) 
    {
        printf ("-> ");
        printf("%.*s", max_width,buffer);

    }
    if (opt->option_l)
    {
        printf("\n");
    }
    free(buffer);
    return;
}

int print_directory(char *path, Option *option) {
    DIR *cwd;
    struct dirent *dp;
    char *buffer = (char*)malloc(PATH_MAX);
    struct stat cur_stat;
    int hasValidFiles = 0;

    // failed to open
    if ((cwd = opendir(path)) == NULL) {
        printf("error opening directory: %d\n", errno);
        exit(1);
    }


    struct dirent **namelist;
    int n;
    n = scandir(path, &namelist, NULL, myCompare);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    // printf("%s\n", path);
    // https://stackoverflow.com/questions/4989431/how-to-use-s-isreg-and-s-isdir-posix-macros
    for(int i =0; i < n; i++) 
    {  
        // get_permissions(cur_stat);
        dp = namelist[i]; 
        if (dp->d_name[0] != '.') { 
        

            hasValidFiles = 1;
            //https://stackoverflow.com/questions/19663042/stat-function-call
            // lstat() returns information about a file pointed to by pathname

            strcpy(buffer, path);
            strcat(buffer, "/");
            strcat(buffer, dp->d_name);  
            int result = lstat(buffer, &cur_stat);
            if (result == -1)
            {
                printf("error %d\n", errno);
                //free(buffer);
                exit(1);
            }
            if (option->option_i) {
                get_ino(cur_stat);
            }
            if (option->option_l) {
                get_permissions(cur_stat);
                get_hardlink(cur_stat);
                get_user_info(cur_stat);
                get_group_info(cur_stat);
                get_file_size(cur_stat);
                get_date_time(cur_stat);
            }
            get_filename(dp->d_name, cur_stat, option);
        }
    }
    // https://comp.soft-sys.ace.narkive.com/MD1oHGGe/ace-users-ace-os-scandir-is-producing-memory-leak
    n = n-1;
    while(n--) {
        free(namelist[n]);
    }

    // printf("\n");
    free(dp);
    free(buffer);
    free(namelist);
    closedir(cwd);
    return hasValidFiles;
}


void print_file(char* file_path, Option* option) { 
    struct stat s_stat;
    int file;

    if (option->option_i) {
        get_ino(s_stat);
     
    }
    if (option->option_l) 
    {
        get_permissions(s_stat);
        get_hardlink(s_stat);
        get_user_info(s_stat);
        get_group_info(s_stat);
        get_file_size(s_stat);
        get_date_time(s_stat);
    }
    get_filename(file_path, s_stat, option);
     
    printf("\n");
}

void recursive_print(char *basePath, Option *option) {
    struct dirent *dp;
    DIR *dir;
    struct stat buf;
    char *path = (char*)malloc(PATH_MAX);
    int printedFiles;

    if((dir = opendir(basePath)) == NULL) {
        perror ("Cannot open.");
        return;
    }
    
    if((strcmp(basePath, ".")) == 0) {
        printf(".:\n");
    } else {
        printf("%s:\n", basePath);
    }
    
    printedFiles = print_directory(basePath, option);

    rewinddir(dir);
    struct dirent **namelist;
    int n = scandir(basePath, &namelist, NULL, myCompare);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    // https://stackoverflow.com/questions/4989431/how-to-use-s-isreg-and-s-isdir-posix-macros
 
    for(int i =0; i < n; i++) {  
        dp = namelist[i];   
        if (dp->d_name[0] == '.') { 
            continue;     
        }
        // memset(path, 0, sizeof(path));
        if (dp->d_name[0] == '.') { 
            continue;     
        }       

        strcpy(path, dp->d_name);
        if(basePath != NULL) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);  

            if((lstat(path, &buf)) != 0) {
                printf("%s: stat error: %d\n", dp->d_name, errno);
            }     
        } else if((lstat(path, &buf)) != 0) {
            printf("%s: stat error: %d\n", dp->d_name, errno);
        }

        if(S_ISDIR(buf.st_mode)) {
            //https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
            if(basePath != NULL) {
                if(printedFiles)
                {
                    if(option->option_R && !option->option_l)
                    {
                        printf("\n\n");
                    }
                    else
                    {
                        printf("\n");
                    }
                    
                    
                }
                strcpy(path, basePath);
                strcat(path, "/");
                strcat(path, dp->d_name);  
            }
            recursive_print(path, option);
        }
    }
    n = n-1;
    while(n--) {
        free(namelist[n]);
    }
    closedir(dir);
    free(path);
    free(namelist);
    free(dp);
}

int main (int argc, char *argv[]) {

    int count = 0;
    char *path = (char*)malloc(PATH_MAX);
    char cwd[PATH_MAX];
    Option *option = malloc(sizeof(Option));
    get_options(argc, argv, option); 

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