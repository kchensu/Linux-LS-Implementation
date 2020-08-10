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

int myCompare(const struct dirent ** dir1, const struct dirent **dir2) {
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

int myStringCmp (const void* str1, const void* str2) {
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