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
int valid_link(char*); 
int added_options(char*);
void get_options(const int, char **argv, Option*); 
void get_ino(struct stat); 
void get_permissions(struct stat);
void get_hardlink(struct stat);
void get_user_info(struct stat); 
void get_group_info(struct stat); 
void get_file_size(struct stat); 
void get_date_time(struct stat); 
void get_filename(char*, struct stat, Option *); 
void print_file(char*,Option*); 
int print_directory(char*, Option*);

extern int optind, opterr, optopt;
static int count_opts = 0;
static int non_opts = 0;
void get_options(const int argc, char **argv, Option* opt) {
    int choice;
    struct stat s_stat;

    while((choice = getopt(argc, argv, "+ilR")) != -1)
    {   
        switch(choice) {
            case 'i':  
                opt->option_i = 1;
                count_opts++;
                break;
            case 'l':  
                opt->option_l = 1;
                count_opts++;
                break;
            case 'R':  
                opt->option_R = 1;
                count_opts++;
                break;
            default:
                exit(1);  
            
        }
    }
    for (int i = optind; i < argc; i++)
    {
        // printf("NON ARGUMENTS %s\n", argv[i]);
        non_opts++;
        
    }

}

int added_options(char* argv) {
    int dash_index = (int)(strchr(argv, '-') - argv); 
    if (dash_index == 0 && valid_directory(argv))
    {
        return 0;
    }
    if (dash_index == 0 && valid_file(argv))
    {
        return 0;
    }
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

void get_filename(char* name,struct stat s_stat, Option *opt) {
    char *buffer = (char*)malloc(PATH_MAX);
    struct stat temp_stat;
    int max_width = 512; 
    //check if is afile

    if (S_ISREG(s_stat.st_mode)) 
    {
        if (opt->option_R)
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
        if (opt->option_R)
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
    
    readlink(name, buffer, PATH_MAX);
    lstat(buffer, &temp_stat);

    //if is a link, then we will need to print out the file/dir its poiting to.
    if (S_ISLNK(s_stat.st_mode) && opt->option_l) 
    {
        printf ("-> ");
        // printf("buffer : %s\n", buffer);
        // printf("name : %s\n", name);
        // check if is a file
        
        if (S_ISREG(temp_stat.st_mode))
        {
            
            if (temp_stat.st_mode & S_IXOTH) 
            {
                printf("%.*s", max_width, buffer); 
               
            }
            
            printf("%.*s", max_width, buffer);
        }
        if (S_ISLNK(temp_stat.st_mode))
        {
            printf("%.*s", max_width, buffer);
          
        }

        //check if is a directory
        if (S_ISDIR(temp_stat.st_mode)) 
        {
            printf("%.*s", max_width,buffer);
        }
    }
    if (opt->option_l)
    {
        printf("\n");
    }
    free(buffer);
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

int myCompare(const struct dirent ** dir1, const struct dirent **dir2) {
    // char* str1 = (*(const struct dirent **)dir1)->d_name;
    // char* str2 = (*(const struct dirent **)dir2)->d_name;
    int sum1;
    int sum2;
    int minStrLength;
    sum2 = 0;
    sum1 = 0;
    char buf1[PATH_MAX];
    char buf2[PATH_MAX];
    int nbr1 = 0;
    int nbr2 = 0;

    strcpy(buf1,(*(const struct dirent **)dir1)->d_name);
    strcpy(buf2,(*(const struct dirent **)dir2)->d_name);
    int strLength1 = strlen(buf1);
    int strLength2 = strlen(buf2);

    //find the bounds for the loop
    if(strLength1 <= strLength2) {
        minStrLength = strLength1;
    } else {
        minStrLength = strLength2;
    }
    
    // loop through and compare characters
    for(int i = 0; i < minStrLength; i++) {
        if(buf1[i] < buf2[i]) {
            return -1;
        } else if (buf1[i] > buf2[i]) {
            return 1;
        }
    }
    return strcmp(buf1, buf2);
}

int print_directory(char *path, Option *option) {
    DIR *cwd;
    struct dirent *dp;
    char *buffer = (char*)malloc(255);
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

void recursicvePrint(char *basePath, Option *option) {
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
    printf("had valiude files?: %d", printedFiles);

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
                if(printedFiles) {
                    printf("\n\n");
                }
                strcpy(path, basePath);
                strcat(path, "/");
                strcat(path, dp->d_name);  
            }
            recursicvePrint(path, option);
        }
    }
    
    closedir(dir);
    free(path);
    free(namelist);
    free(dp);
}

int myStringCmp (char* str1, char* str2) {
    char buf1[PATH_MAX];
    char buf2[PATH_MAX];

    strcpy(buf1, str1);
    strcpy(buf2, str2);


    int strLength1 = strlen(buf1);
    int strLength2 = strlen(buf2);


    int minStrLength;

    //find the bounds for the loop
    if(strLength1 <= strLength2) {
        minStrLength = strLength1;
    } else {
        minStrLength = strLength2;
    }

    
    // loop through and compare characters
    for(int i = 0; i < minStrLength; i++) {
        if(buf1[i] < buf2[i]) {
            return -1;
        } else if (buf1[i] > buf2[i]) {
            return 1;
        }
    }
    return strcmp(buf1, buf2);
}



int main (int argc, char *argv[]) {
    int count = 0;
    char *path = (char*)malloc(PATH_MAX);
    char cwd[PATH_MAX];
    memset(cwd, 0, PATH_MAX); 
    Option *option = malloc(sizeof(Option));
    get_options(argc, argv, option); 
   

   char temp[1000];
    int dif;
    for (int i = 0; i< argc; i++) {
        for(int j = i + 1; j< argc; j++) {
            dif = myStringCmp(argv[i], argv[j]);
            if(dif > 0) {
                strcpy(temp, argv[i]);
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
        // printf("here are the arguments %d: , %s\n", i, argv[i]);
       
        // if (added_options(argv[i])) 
        //     continue;
         

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
            recursicvePrint(".", option);
        } else {
            
            print_directory(cwd, option);
        }    
    }

    // [ARGUEMENTS]
    for (int j = argc - non_opts; j < argc; j++) 
    {   
        


        // skips the option arguments ie. -i, -l etc..
        // only print directory and file name
        
        // if (added_options(argv[j])) 
        // {
        //     continue; 
        // }
             
        path = argv[j];
        if (single_file(path) || valid_link(path))
        {
            print_file(path, option);
        } else if(directory(path)) {
            if(option->option_R) 
            {
                recursicvePrint(path, option);
            } 
            else
            {  
                printf("%s:\n", path);
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
    // free(path);
    free(option);
    printf("\n");

    return 0;
}