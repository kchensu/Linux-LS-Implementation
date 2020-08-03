#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_ino(struct stat s_stats)
{
  printf("%ld ", s_stats.st_ino);
}

void print_rwx(struct stat s_stats) {
    mode_t permission = s_stats.st_mode;
    //user permission
    printf((permission & S_IRUSR) ? "r" : "-");
    printf((permission & S_IWUSR) ? "w" : "-");
    printf((permission & S_IXUSR) ? "x" : "-");
    // group permission
    printf((permission & S_IRGRP) ? "r" : "-");
    printf((permission & S_IWGRP) ? "w" : "-");
    printf((permission & S_IXGRP) ? "x" : "-");
    // org permission
    printf((permission & S_IROTH) ? "r" : "-");
    printf((permission & S_IWOTH) ? "w" : "-");
    printf((permission & S_IXOTH) ? "x" : "-");
}

void get_link(struct stat s_stat) {
    printf(" %ld ", s_stat.st_nlink );
}

void get_user(struct stat s_stat) {
    // check for null
    printf(" %s ", getpwuid(s_stat.st_uid)->pw_name);
}

void get_group(struct stat s_stat) {
    //check for null
    printf(" %s ", getgrgid(s_stat.st_gid)->gr_name);
}

void get_file_size(struct stat s_stat) {
    printf("%ld", s_stat.st_size);
}

// need to align text
void get_date_time(struct stat s_stat) {
    char *buffer = (char*)malloc(225 * sizeof(char));
    struct tm *time;
    time_t time_s = s_stat.st_ctime;
    time = localtime(&time_s);
    strftime(buffer,255,"    %b %d %H:%M",time);
    printf("%s",buffer);
}







int main(int argc, char *argv[])
{
  DIR *dir;
  struct dirent *dp;
  struct stat s_stats;
  char *file_name;

  // only ./UnixLs was entered, therefore ls is run on the cwd "."
  if (argc == 1)
  {
    dir = opendir(".");
    while ((dp = readdir(dir)) != NULL)
    {
      if (dp->d_name[0] != '.')
      {
        file_name = dp->d_name;
        printf("%s\n", file_name);
        
      }
    }

  }
    closedir(dir);

    if (argc >= 2)
    {
        char *option = argv[1];
        char *path = (char *)malloc(255 * sizeof(char));
        char *fullpath = (char *)malloc(255 * sizeof(char));
        char *filepath;
        char *file_name;
        char *link_name;


        if (option[0] == '-' && (option[1] == 'i' || option[1] == 'l'))
        {
            if (argc >= 3)
            {
               
                char *check = argv[2];
                if (!strcmp(check, "-i") || strcmp(check, "-l"))
                {
                    strncat(option, &check[1], 1);
                } 
            }
            if (!strcmp(argv[argc - 1], option))
            {
                filepath = ".";
            }
            else
            {
                strcpy(path, argv[argc - 1]);
                filepath = path;
            }
        }
        else
        {
            strcpy(path, option);
            filepath = path;
        }
        dir = opendir(filepath);

        if (dir == NULL)
        {
            printf("No such file or directory\n");
            exit(1);
        }
      

        while ((dp = readdir(dir)) != NULL)
        {
            
            strcpy(fullpath, filepath);
            strcat(fullpath, "/");
            strcat(fullpath, dp->d_name);
            
            file_name = dp->d_name;
            if (stat(fullpath, &s_stats) == 0)
            {
                if (file_name[0] != '.')
                {
                    if (option[1] == 'i' && option[2] != 'l')
                    {
                        print_ino(s_stats);
                    }
                    else if (option[1] == 'l' && option[2] != 'i')
                    {
                        print_rwx(s_stats);
                        get_link(s_stats);
                        get_user(s_stats);
                        get_group(s_stats);
                        get_file_size(s_stats);
                        get_date_time(s_stats);
                        
                    }
                    else if ((option[1] == 'i' && option[2] == 'l' )|| (option[1] == 'l' && option[2] == 'i'))
                    {

                        print_ino(s_stats);
                        print_rwx(s_stats);
                        get_link(s_stats);
                        get_user(s_stats);
                        get_group(s_stats);
                        get_file_size(s_stats);
                        get_date_time(s_stats);
                    }
                    else if ((option[1] == 'i' && option[5] == 'l' )|| (option[1] == 'l' && option[5] == 'i'))
                    {

                        print_ino(s_stats);
                        print_rwx(s_stats);
                        get_link(s_stats);
                        get_user(s_stats);
                        get_group(s_stats);
                        get_file_size(s_stats);
                        get_date_time(s_stats);
                    }
                    else
                    {
                        printf("error: %d\n", errno);
                        exit(1);
                    }
                    printf("%10s\n", file_name);
                }
            }
            else
            {
                exit(1);
            }
            

        }
        free(link_name);
        free(path);
        free(fullpath);
        closedir(dir);
        

       
        
    }



    return 0;




    
  }

 
  