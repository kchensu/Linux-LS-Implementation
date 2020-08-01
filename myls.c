#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *dp;
    struct stat buf;
    char *filename;
     if (argc == 1) 
     {
         dir = opendir(".");
         while ((dp = readdir(dir)) != NULL)
         {
             if (dp->d_name[0] != '.')
             {
                 filename = dp->d_name;
                 printf("%s\n", filename);
             }
         }

     }
     closedir(dir);

}