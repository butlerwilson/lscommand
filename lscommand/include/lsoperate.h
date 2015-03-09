/*
 * =========================================================================
 *
 *       Filename:  lsoperate.h
 *
 *    Description:  Define ls how to works.
 *
 *        Version:  1.0
 *        Created:  05/20/2014 09:22:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#ifndef _LSOPERATE_H_
#define _LSOPERATE_H_

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

//define the colors
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#define FLASH        "\033[5m"

/*define the data structure*/
#define PARA_A 0X01
#define PARA_I 0X02
#define PARA_L 0X04
#define PARA_R 0X08
#define PARA_S 0X10
#define PARA_T 0X20
#define PARA_U 0X40

#define MODELEN		16
#define TIMELEN		16
#define NAMELEN		32
#define COLLEN		16

/*first time I defined the size is 10240. But after saveral
 *recursion, it occurred segment fault. After about one day's debugging
 *,I ensured that all my logical dealling is all right. At last, I guess
 *that the stack is not enough for me. I have to reduce the file number
 *into 64. Here, if the file number is beyound the 64 in one directory.
 *For safe, we will ignore the rest file. This is a bug. I will find a
 *better way to solve this bug.
 */
#define MAX_FILE	64
#define MAX_PATH	256

struct file_info {
	size_t	fi_inode;
	size_t	fi_blocks;
	char	fi_mode[MODELEN];
	size_t	fi_links;
	
	char	fi_username[NAMELEN];
	char	fi_groupname[NAMELEN];
	
	size_t	fi_size;
	
	char	fi_atime[TIMELEN];
	char	fi_mtime[TIMELEN];

	char	fi_color[COLLEN];
	char	fi_fname[MAX_PATH];
	char	fi_lname[MAX_PATH];
	char	fi_endcolor[COLLEN];
};

#define TL	sizeof(struct file_info *)

int main_entry(const char *pathname, size_t size, void *options);

ssize_t get_last_modified_time(char *line, time_t tm);
ssize_t get_last_accessed_time(char *line, time_t tm);
ssize_t get_size2line(char *line, off_t size);
ssize_t get_group_name2line(char *line, gid_t gid);
ssize_t get_user_name2line(char *line, uid_t uid);
ssize_t get_links2line(char *line, struct stat *st);
ssize_t get_mode2line(char *line, mode_t mode);

void assignment_value(struct file_info *fi, struct stat st, int options);
void print_cur_path(char *curpath, const char *pathname);

int cmpare_filetime(const void *p1, const void *p2);
int cmpare_filename(const void *p1, const void *p2);
int list_command_sort(struct file_info **fileinfo, int count, int options);

int list_dir_info(const char *pathname, int options);
int list_dirs_info(const char *path, struct file_info **filename,
		int count, size_t *blocks, int options);

int list_long_info(struct file_info *fi, int options);
void set_print_color(char *color, mode_t mode);

void set_typemode2line(char *line, mode_t mode);
void set_mode2line(char *line, int usermode, char ch,  mode_t mode);

void construct_options(void *options, int *paramter);

int cmpare_filelen(struct file_info **filename, int count, size_t *fnlen,
		size_t *tmpfnlen);
int list_span_print(struct file_info **fileinfo, int count);

int errors(const char *pathname, int errorno);

#endif
