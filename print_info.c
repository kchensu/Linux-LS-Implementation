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
