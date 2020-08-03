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
    printf("ARGS:\n");
    for(int i = 0; i < argc; i++) {
        printf("argc %d: %s\n", i, argv[i]);
    }


    DIR *dir;
    struct dirent *dp;
    struct stat buf;
    char *filename;
    dir = opendir(argv[2]);

    printf("\nMy ls:\n");

    if(strcmp(argv[1],"-R ")) {
        while ((dp = readdir(dir)) != NULL) {
            stat(dp->d_name, &buf);
            if (dp->d_name[0] != '.') {
                //filename = dp->d_name;
                printf("%s ", dp->d_name);
            }
        }
        printf("\n\n");

        //do the second read
        rewinddir(dir);
        while ((dp = readdir(dir)) != NULL) {
            stat(dp->d_name, &buf);
            if (dp->d_name[0] != '.') {
                if(S_ISDIR(buf.st_mode)) {
                    printf("./%s\n", dp->d_name);
                    DIR *newDir;
                    struct dirent *newDp;
                    struct stat newBuf;
                    newDir = opendir(dp->d_name);
                    while(newDp = readdir(newDir) != NULL) {
                        stat(newDp->d_name, &newBuf);
                        if (newDp->d_name[0] != '.') {
                            //filename = dp->d_name;
                            printf("%s ", newDp->d_name);
                        }
                    }
                }
            }
        }
    }
    
    closedir(dir);

}