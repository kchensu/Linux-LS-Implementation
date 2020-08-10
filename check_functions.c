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

int valid_directory(char *path) {
    DIR *dir; 
    if ((dir = opendir(path)) == NULL) {
        //printf(" directory error\n");
        return 0;
    } else {
        return 1;
    }
}

int single_file(char *path) {
    struct stat s_stat;
    lstat(path, &s_stat); 
    // This macro returns non-zero if the file is a regular file.
    if (S_ISREG(s_stat.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int valid_file(char* path) {
    int file;
    if((file = open(path, O_RDONLY)) <= -1) {
        return 0;
    }
    return 1;
}

int directory(char *path) 
{
    struct stat s_stat;
    lstat(path, &s_stat); 
    //This macro returns non-zero if the file is a directory.
    if (S_ISDIR(s_stat.st_mode)) {
        return 1;
    }
    return 0;
}

int valid_link(char* path)
{
    struct stat s_stat;
    lstat(path, &s_stat);
    if(S_ISLNK(s_stat.st_mode))
    {
        return 1;
    }
    return 0;
}

