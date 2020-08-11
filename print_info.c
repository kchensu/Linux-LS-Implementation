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

# define DATE_BUFFER 512

void get_ino(struct stat s_stat) {
    printf("%llu ",(unsigned long long) s_stat.st_ino);
}

void get_permissions(struct stat path_stats) {
    mode_t mode = path_stats.st_mode;
    // get file type
    printf((S_ISDIR(path_stats.st_mode)) ? "d" : "");
    printf((S_ISREG(path_stats.st_mode)) ? "-" : "");
    printf((S_ISLNK(path_stats.st_mode)) ? "l" : "");
    // user permissions
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    // group permissions
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    // other permissions
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}
void get_hardlink(struct stat s_stat)
{
    int max_width = 2;
    printf("%*ld ", max_width, s_stat.st_nlink);
}
void get_user_info(struct stat s_stat) 
{   
    int max_width = 2;
    printf("%*s ", max_width, getpwuid(s_stat.st_uid)->pw_name);
}

void get_group_info(struct stat s_stat) 
{
    int max_width = 2;
    printf("%*s ",max_width, getgrgid(s_stat.st_gid)->gr_name);
}

void get_file_size(struct stat s_stat) 
{   
    printf("%5ld ", s_stat.st_size);
}

void get_date_time(struct stat s_stat) {
    struct tm *time;
    char *buffer = (char*)malloc(255);
    int max_width = 10;
    time_t time_s = s_stat.st_ctime;
    time = localtime(&time_s);
    strftime(buffer, DATE_BUFFER ,"%b %_d %Y %H:%M ",time);
    printf("%*s", max_width, buffer);
    free(buffer);
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