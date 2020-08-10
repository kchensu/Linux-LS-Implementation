#ifndef _PRINT_INFO_H_
#define _PRINT_INFO_H_

void get_ino(struct stat);
void get_permissions(struct stat);
void get_hardlink(struct stat);
void get_user_info(struct stat); 
void get_group_info(struct stat); 
void get_file_size(struct stat); 
void get_date_time(struct stat);  

#endif