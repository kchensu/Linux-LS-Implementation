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
#include <stdlib.h>
#include <stdio.h>

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

void printInode(struct stat fileStats)
{
  printf("%20lu ", fileStats.st_ino);
}

int main(int argc, char *argv[])
{
  DIR *dir;
  struct dirent *dp;
  struct stat buf;
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

    closedir(dir);
  }

  // must check for [OPTION], [PATH], etc.
  if (argc >= 2)
  {
    char *option_or_path = argv[1];
    char *path = (char *)malloc(255 * sizeof(char));
    char *fullpath = (char *)malloc(255 * sizeof(char));
    char *filepath;
    char *file_name;
    char *link_name;
    ssize_t r;

    if (!strcmp(option_or_path, "ls"))
    {
      printf("ERROR: invalid formatting, this program runs by executing ./UnixLs [OPTION] [PATH].\n");
      printf("EXAMPLE: ./UnixLs -il ..\n");
      exit(1);
    }

    // checking that -i, -l, -il, or -li was used
    if (option_or_path[0] == '-' && (option_or_path[1] == 'i') || option_or_path[1] == 'l')
    {
      // check for edgecases "./UnixLs -i -l [PATH]" or "./UnixLs -l -i [PATH]"
      if (argc >= 3)
      {
        char *check_il_or_li_edgecase = argv[2];
        if (!strcmp(check_il_or_li_edgecase, "-i") || !strcmp(check_il_or_li_edgecase, "-l"))
        {
          strncat(option_or_path, &check_il_or_li_edgecase[1], 1);
        }
      }

      // [PATH] was not given, therefore do ls with [OPTION] on cwd
      if (!strcmp(argv[argc - 1], option_or_path))
      {
        filepath = ".";
      }
      else
      {
        // copy [PATH] to filepath
        strcpy(path, argv[argc - 1]);
        filepath = path;
      }
    }
    else
    {
      // no [OPTION] specified, just copy the path
      strcpy(path, option_or_path);
      filepath = path;
    }

    dir = opendir(filepath);

    // illegal path; doesn't contain '/', '~', '.', '..'
    if (dir == NULL)
    {
      printf("File path [%s] was not found!\n", filepath);
      exit(1);
    }
    else
    {
      while ((dp = readdir(dir)) != NULL)
      {
        strcpy(fullpath, filepath);
        strcat(fullpath, "/");
        strcat(fullpath, dp->d_name);

        file_name = dp->d_name;

        // file exists
        if (stat(fullpath, &buf) == 0)
        {
          if (file_name[0] != '.')
          {
            if (option_or_path[1] == 'i' && option_or_path[2] != 'l')
            {
              printInode(buf);
            }

            else if ((option_or_path[1] == 'i' && option_or_path[2] == 'l') || (option_or_path[1] == 'l' && option_or_path[2] == 'i'))
            {
              printInode(buf);
      
            }

            
            printf("%s", file_name);
            

            // handle symbolic link
            if (lstat(fullpath, &buf) == -1)
            {
              printf("\n");
            }
            else
            {
              link_name = malloc(buf.st_size + 1);
              r = readlink(fullpath, link_name, buf.st_size + 1);

              if (r < 0 || r > buf.st_size)
              {
                printf("\n");
              }
              else
              {
                link_name[buf.st_size] = '\0';
                printf(" -> %s\n", link_name);
              }

              free(link_name);
            }
          }
        }
        else
        {
          printf("Stat failed: %s\n\n", strerror(errno));
        }
      }
    }

    free(path);
    free(fullpath);
    closedir(dir);
  }

  return 0;
}







