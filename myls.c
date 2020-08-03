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

char *getUserName(uid_t uid)
{
  struct passwd *pw = NULL;
  pw = getpwuid(uid);

  if (pw)
  {
    return pw->pw_name;
  }
  else
  {
    return "null";
  }
} 

char *getGroupName(gid_t grpNum)
{
  struct group *grp;
  grp = getgrgid(grpNum);

  if (grp)
  {
    return grp->gr_name;
  }
  else
  {
    return "null";
  }
}

void print_ino(struct stat s_stats)
{
  printf("%ld", s_stats.st_ino);
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

    if (argc == 2)
    {
        char *option_or_path = argv[1];
        char *path = (char *)malloc(255 * sizeof(char));
        char *fullpath = (char *)malloc(255 * sizeof(char));
        char *filepath;
        char *file_name;
        char *link_name;
        ssize_t r;

        if (option_or_path[0] == '-' && (option_or_path[1] == 'i' || option_or_path[1] == 'l'))
        {
            if (argc == 3)
            {
                char *check = argv[2];
                if (!strcmp(check, "-i") || strcmp(check, "-l"))
                {
                    strncat(option_or_path, &check[1], 1);
                } 
            }
            if (!strcmp(argv[argc - 1], option_or_path))
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
            strcpy(path, option_or_path);
            filepath = path;
        }
        dir = opendir(filepath);
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
                    if (option_or_path[1] == 'i' && option_or_path[2] != 'l')
                    {
                        print_ino(s_stats);
                    }
                    else if (option_or_path[1] == 'l' && option_or_path[2] != 'i')
                    {
                        print_ino(s_stats);
                    }
                    else if ((option_or_path[1] == 'i' && option_or_path[2] == 'l' )|| (option_or_path[1] == 'l' && option_or_path[2] == 'i'))
                    {
                        print_ino(s_stats);
                    }
                    printf("%15s\n", file_name);
                }
            }

        }
        free(link_name);
        free(path);
        free(fullpath);
        closedir(dir);
        

       
        
    }



    return 0;




    
  }

 
  