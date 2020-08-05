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

# define DATE_BUFFER 512

typedef struct Options {
    int option_i;
    int option_l;
    int option_R;
    int option_not_hidden;
} Option;


int valid_file(char*); 
int valid_directory(char*); 
int single_file(char*); 
int directory(char*); 
int added_options(char*);
void get_options(const int, char **argv, Option*); 
void get_ino(struct stat); 
void get_permissions(struct stat);
void get_hardlink(struct stat);
void get_user_info(struct stat); 
void get_group_info(struct stat); 
void get_file_size(struct stat); 
void get_date_time(struct stat); 
void get_filename(char*, struct stat); 
void print_file(char*,Option*); 
void print_directory(char*, Option*); 


void get_options(const int argc, char **argv, Option* opt) {
    int choice;
    while((choice = getopt(argc, argv, "ilR")) != -1) {  
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
            // invalid option, exit program
                printf("invalid option");
                exit(1);     
        }
    }
}

int added_options(char* argv) {
    int dash_index = (int)(strchr(argv, '-') - argv); 
    if (dash_index == 0) 
        return 1;
    return 0;
}
void get_ino(struct stat s_stat) {
    printf("%ld ", s_stat.st_ino);
}
// https://jameshfisher.com/2017/02/24/what-is-mode_t/
void get_permissions(struct stat path_stats) {
    mode_t mode = path_stats.st_mode;
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
    printf(" %ld ", s_stat.st_nlink);
}
void get_user_info(struct stat s_stat) {
    printf("%s ", getpwuid(s_stat.st_uid)->pw_name);
}

void get_group_info(struct stat s_stat) {
    printf("%s ", getgrgid(s_stat.st_gid)->gr_name);
}

void get_file_size(struct stat s_stat) {
    printf("%5ld  ", s_stat.st_size);
}

void get_date_time(struct stat s_stat) {
    struct tm *time;
    char *buffer = (char*)malloc(255);
    time_t time_s = s_stat.st_ctime;
    time = localtime(&time_s);
    strftime(buffer, DATE_BUFFER ,"%b %d %Y %H:%M",time);
    printf("%s   ",buffer);
    free(buffer);
}

void get_filename(char* name, struct stat s_stat) {
    printf("%s\n",name);
}
// //https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html
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

int valid_directory(char *path) {
    DIR *dir; 
    if ((dir = opendir(path)) == NULL) {
        //printf(" directory error\n");
        return 0;
    } else {
        closedir;
        return 1;
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

void print_directory(char *path, Option *option) {
    DIR *cwd;
    struct dirent *dp;
    char *buffer = (char*)malloc(255);
    struct stat cur_stat;

    /* Do nothing if failed to open directory as error message has already been printed */
    if ((cwd = opendir(path)) == NULL) {
        printf("error opening directory");
        exit(1);
    }
    // https://stackoverflow.com/questions/4989431/how-to-use-s-isreg-and-s-isdir-posix-macros
    while((dp = readdir(cwd)) != NULL) {     
        if (dp->d_name[0] == '.') { 
            continue;     
        }
        //https://stackoverflow.com/questions/19663042/stat-function-call
        // stat() returns information about a file pointed to by pathname
        strcpy(buffer, path);
        strcat(buffer, "/");
        strcat(buffer, dp->d_name);  
        int result = stat(buffer, &cur_stat);
        if (result == -1)
        {
            printf("error %d\n", errno);
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
        get_filename(dp->d_name, cur_stat);
    }
    printf("\n");
    free(dp);
    free(buffer);
    closedir(cwd);
}

void print_file(char* file_path, Option* option) { 
    struct stat s_stat;
    int file;
    
    if (option->option_i) {
        get_ino(s_stat);
     
    }
    if (option->option_l) {
        get_permissions(s_stat);
        get_user_info(s_stat);
        get_group_info(s_stat);
        get_file_size(s_stat);
        get_date_time(s_stat);
    }
    get_filename(file_path, s_stat);
    printf("\n");
}

void recursicvePrint(char *basePath, Option *option) {
    struct dirent *dp;
    DIR *dir;
    struct stat buf;
    char *file_name;
    char *path = (char*)malloc(1000);

    if((dir = opendir(basePath)) == NULL) {
        perror ("Cannot open.");
        return;
    }
    
    if((strcmp(basePath, ".")) == 0) {
        printf(".:\n");
    } else {
        printf("%s:\n", basePath);
    }
 
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_name[0] !='.') {
            file_name = dp->d_name;
            if(basePath != NULL) {
                strcpy(path, basePath);
                strcat(path, "/");
                strcat(path, dp->d_name);  
                if((stat(path, &buf)) != 0) {
                    printf("%s: stat error: %d\n", dp->d_name, errno);
                }     
            } else if((stat(path, &buf)) != 0) {
                printf("%s: stat error: %d\n", dp->d_name, errno);
            }

            if (option->option_i) {
                get_ino(buf);
            }
            if (option->option_l) {
                get_permissions(buf);
                get_hardlink(buf);
                get_user_info(buf);
                get_group_info(buf);
                get_file_size(buf);
                get_date_time(buf);
                printf("%s\n", file_name);
            } else {
                printf("%s ", file_name);
            }     
        }
    }

    rewinddir(dir);
    while ((dp = readdir(dir)) != NULL)
    { 
        memset(path, 0, sizeof(path));
        if (dp->d_name[0] == '.') { 
            continue;     
        }       
      
        strcpy(path, dp->d_name);
        
        if(basePath != NULL) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);  

            if((stat(path, &buf)) != 0) {
                printf("%s: stat error: %d\n", dp->d_name, errno);
            }     
        } else if((stat(path, &buf)) != 0) {
            printf("%s: stat error: %d\n", dp->d_name, errno);
        }

        if(S_ISDIR(buf.st_mode)) {
            //https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
            if(basePath != NULL) {
                printf("\n\n");
                strcpy(path, basePath);
                strcat(path, "/");
                strcat(path, dp->d_name);  
            }
            recursicvePrint(path, option);
        }
    }

    free(path);
    closedir(dir);
}

int main (int argc, char *argv[]) {
    int count = 0;
    char *path = (char*)malloc(1000);
    char cwd[255]; 
    Option *option = malloc(sizeof(Option));
    int file_path_arg;

    get_options(argc, argv, option); 

    // loop through all args and skips the options[ -i, -l, -il etc...]
    for (int i = 1; i < argc; i++)
    { 
        if (added_options(argv[i])) 
            continue; 

        if (!valid_directory(argv[i]) && !valid_file(argv[i])) {
            printf("ls: cannot access %s", argv[i]);
            printf("No such file or directory\n");
            //exit(1);
        }

        /* Count file or directory argument that exists*/
        else if (directory(argv[i]) || single_file(argv[i])) {
            if (valid_directory(argv[i]) || valid_file(argv[i])) 
            {
                count++;
            }
        }
    }

    // https://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html
    // no options given
    if (count == 0) {
        // getcwd - get the pathname of the current working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            printf(" get cwd error %d\n", errno); 
            exit(1);
        }
        if(option->option_R) {
            recursicvePrint(".", option);
        } else {
            print_directory(cwd, option);
        }    
    }

    // [ARGUEMENTS]
    for (int j = 1; j < argc; j++) 
    {   
        // skips the option arguments ie. -i, -l etc..
        // only print directory and file name
        if (added_options(argv[j])) 
        {
            continue; 
        }     
        path = argv[j];
        if (single_file(path))
        {
            print_file(path, option);
        } else if(directory(path)) {
            if(option->option_R) {
                recursicvePrint(path, option);
            } else  {
                print_directory(path, option);
            }
        }
            
    }
    
    // [ARGUEMENT 2]
    // for (int k = 1; k < argc; k++) 
    // { 
    //     // skips the option arguments ie. -i, -l etc..
    //     // only print directory and file name
    //     if (added_options(argv[k])) 
    //     {
    //         continue;
    //     }
    //     path = argv[k];
    //     if (directory(path)) {
    //         if (count > 1)
    //         {
    //             printf("whhhhhhhhhhhhhhhhhhatt?%s:", path);
    //         }   
    //         print_directory(path, option);
    //     }
    // }

    return 0;
}