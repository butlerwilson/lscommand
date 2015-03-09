/*
 * =========================================================================
 *
 *       Filename:  lsoperate.c
 *
 *    Description:  Implement the ls functions.
 *
 *        Version:  1.0
 *        Created:  05/20/2014 08:39:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#include "lsoperate.h"

		/*ls command implement.*/
/*In the main function, we use different parameters to decide ls how to work
 *The basic function is print the specified path, the deafult is current
 *path. But how to reuse this main_ls function? All ls function will print
 *the files in a order, by time, by alpha or others. And most of them are
 *different colors. Normally, the different file type holds different colors
 *. So the most important is define how to sort them, and print them in
 *different colors. Then we should know where the ls command will run.
 */

//first time to judge if it is file
int _IS_FILE_ = 0;
//store the columns and rows
int column, row;
//store the current path
char curpath[MAX_PATH] = {'\0'};

int main_entry(const char *pathname, size_t size, void *options)
{
	int param = 0;
	int status = 0;
	struct stat st;

	status = stat(pathname, &st);
	status = errors(pathname, status);
	construct_options(options, &param);
	if (S_ISDIR(st.st_mode)) {
		//should recursive visit
		get_term_stat(&column, &row);
		list_dir_info(pathname, param);
	} else {
		//just print the identified file informations
		struct file_info fi;
		assignment_value(&fi, st, param);
		list_long_info(&fi, param);
	}

}

/*Asumming the pathname is ../, and the directory "../" has 3 directies
 *and 2 files. They are:
 *  d_vim d_pro d_game f_passwd f_time.conf
 */
int list_dir_info(const char *pathname, int options)
{
	int i = 0;
	int index = 0;
	int mcount = 2;

	int status = 0;
	struct stat st;
	size_t blocks = 0;
	
	struct dirent *dirs;
	struct file_info *tmpfi = NULL;
	struct file_info **fileinfo = NULL;

	DIR *dir = opendir(pathname);
	errors(pathname, errno);
	fileinfo = (struct file_info **)malloc(sizeof(struct file_info *)
		       	* MAX_FILE);

	if (chdir(pathname) == -1) return 1;
	while ((dirs = readdir(dir)) != NULL) {
		if ((!(options & PARA_A)) && (dirs->d_name[0] == '.'))
			continue;
		tmpfi=(struct file_info *)malloc(sizeof(struct file_info));
		if (index > MAX_FILE) {
			fileinfo = (struct file_info **)realloc(fileinfo, 
					MAX_FILE * mcount);
			mcount += 2;
		}
		
		strncpy(tmpfi->fi_fname, dirs->d_name, MAX_PATH);
		status = stat(tmpfi->fi_fname, &st);
		errors(tmpfi->fi_fname, errno);
		assignment_value(tmpfi, st, options);
		fileinfo[index++] = tmpfi;
		blocks += tmpfi->fi_blocks;
	}
	closedir(dir);

	if (index == 0) return 0;	//no files to show
	list_command_sort(fileinfo, index, options);

	if (options & PARA_R) {		//print simple file informations
		print_cur_path(curpath, pathname);
		list_span_print(fileinfo, index);
		list_dirs_info(curpath, fileinfo, index, &blocks, options);
		chdir("../");	//back to parent directory
	}

	//no -R and no -l print
	if (!(options & PARA_R))
		list_main_print(fileinfo, index, options);

	//free space
	for (i = 0; i < index; i++)
		free(fileinfo[i]);
	free(fileinfo);

	return 0;
}

int list_dirs_info(const char *pathname, struct file_info **fileinfo,
		int count, size_t *blocks, int options)
{
	int i = 0;
	int status = 0;
	struct stat st;
	size_t ncsize = strlen(NONE);

	//first time print the directories
	if (options & (PARA_L | PARA_S)) printf("total:%lu\n", *blocks);
	list_main_print(fileinfo, count, options);

	//second time go into the directories
	for (i = 0; i < count; i++) {
		status = stat(fileinfo[i]->fi_fname, &st);
		status = errors(fileinfo[i]->fi_fname, status);
		if (S_ISDIR(st.st_mode)) {
			if (strcmp(fileinfo[i]->fi_fname, ".") == 0 || 
			strcmp(fileinfo[i]->fi_fname, "..") == 0)
				continue;
			list_dir_info(fileinfo[i]->fi_fname, options);
		}
	}
}

int list_main_print(struct file_info **fileinfo, ssize_t count, int options)
{
	int  i = 0;

	if (options & PARA_L) {
		for (i; i < count; i++)
			list_long_info(fileinfo[i], options);
		return 0;
	}

	if (!(options & PARA_R)) {
		list_span_print(fileinfo, count);
	}

}

void assignment_value(struct file_info *tmpfi, struct stat st, int options)
{

	//assignment to the tmpfi pointor
	tmpfi->fi_inode = st.st_ino;
	tmpfi->fi_blocks = st.st_blocks / 2;
	get_mode2line(tmpfi->fi_mode, st.st_mode);
	tmpfi->fi_links = st.st_nlink;
	get_user_name2line(tmpfi->fi_username, st.st_uid);
	get_group_name2line(tmpfi->fi_groupname, st.st_gid);
	tmpfi->fi_size = st.st_size;
	if (options & PARA_U)
		get_last_modified_time(tmpfi->fi_mtime, st.st_mtime);
	get_last_accessed_time(tmpfi->fi_atime, st.st_atime);
	set_print_color(tmpfi->fi_color, st.st_mode);
	strcpy(tmpfi->fi_endcolor, NONE);
}

int list_span_print(struct file_info **fileinfo, int count)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int tmp = 0;

	int rows = 0;
	int columns = 0;

	char color[COLLEN] = {'\0'};
	char abspath[MAX_PATH] = {'\0'};
	size_t maxcollen[80] = {0};
	size_t fnlen[MAX_FILE] = {0};
	size_t tmpfnlen[MAX_FILE] = {0};

	cmpare_filelen(fileinfo, count, fnlen, tmpfnlen);

	//just got the columns and rows, nothing later
	for (i = 0; i < count; i++) {
		tmp = tmp + fnlen[i] + 1;
		if (tmp < column)
			columns++;
		else
			break;
	}
	rows = (int)(count / columns) + (count % columns ? 1 : 0);

	//find the per columns's maxinum length
	for (j = 0; j < columns; j++) {
		for (i = 0; i < rows; i++) {
			if (tmpfnlen[j * rows + i] > maxcollen[j])
				maxcollen[j] = tmpfnlen[j * rows + i];
		}
	}


	for (i = 0; i < rows; i++) {
		for (j = 0; (j < columns) && (j * rows + i < count); j++) {
			struct file_info *tmp = fileinfo[j * rows + i];
			printf("%s%s%s", tmp->fi_color, tmp->fi_fname,
					tmp->fi_endcolor);
			for (k = 0; k <= maxcollen[j] - tmpfnlen[j * rows + i] + 1; k ++)
				printf(" ");
		}
		printf("\n");
	}
}

int cmpare_filelen(struct file_info **fileinfo, int count, size_t *fnlen,
		size_t *tmpfnlen)
{
	int i = 0;
	int j = -1;
	int k = 0;
	size_t len = 0;

	//insert sort method
	for (i; i < count; i++) {
		len = strlen(fileinfo[i]->fi_fname);
		tmpfnlen[i] = len;
		j += 1;
		for (k = j; k >= 0; k--) {
			if (len > fnlen[k]) {
				fnlen[k + 1] = fnlen[k];
				fnlen[k] = len;
			} else {
				fnlen[k+1] = len;
				break;
			}
		}
	}
}

void print_cur_path(char *curpath, const char *pathname)
{
	size_t size = 0;

	size = strlen(curpath);
	if ((size == 0) || (curpath[size - 1] == '/'))
		sprintf(curpath + size, "%s", pathname);
	else
		sprintf(curpath + size, "/%s", pathname);
	printf("%s%s%s:\n", BLUE, curpath, NONE);
}

int get_term_stat(int *column, int *row)
{
	char *col = 0;
	struct 	winsize win;

	col = getenv("COLUMN");
	if (col != NULL && *col != '\0') {
		*column = atoi(col);
		return 0;
	}

	if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &win)) != -1 && 
		win.ws_col > 0) {
		*column = win.ws_col;
	}

	return 0;
}

int list_command_sort(struct file_info **fileinfo, int count, int options)
{
	if (options & PARA_U && !(options & PARA_L)) //sort with atime
		qsort(fileinfo, count, TL, cmpare_filetime);
	else			//sort with filename
		qsort(fileinfo, count, TL, cmpare_filename);
}

int cmpare_filetime(const void *p1, const void *p2)
{
	struct file_info *p = (struct file_info *)p1;
	struct file_info *q = (struct file_info *)p2;
	

	return strcmp(p->fi_atime, q->fi_atime);
}

int cmpare_filename(const void *p1, const void *p2)
{
	struct file_info *p, *q;
	char *name1, *name2;

	p = (struct file_info *)p1;
	q = (struct file_info *)p2;
	
	name1 = p->fi_fname;
	name2 = p->fi_fname;

	if (name1[0] == '.')
		name1++;
	if (name2[0] == '.')
		name2++;
	return strcasecmp(p->fi_fname, q->fi_fname);
}

int list_long_info(struct file_info *fi, int options)
{
	if ((!(options & PARA_A)) && fi->fi_fname[0] == '.')
		return -1; //not printf prefix with '.'
	if (options & PARA_I) printf("%-7lu ", fi->fi_inode);
	if (options & PARA_S) printf("%3lu ", fi->fi_blocks);
	printf("%s%-2.lu%s%s%5lu", fi->fi_mode, fi->fi_links,
		fi->fi_username, fi->fi_groupname, fi->fi_size);
	if (options & PARA_U)
		printf(" %13s", fi->fi_mtime);
	else
		printf(" %13s", fi->fi_atime);
	printf("%s%s%s\n", fi->fi_color, fi->fi_fname, fi->fi_endcolor);

	return 0;
}

void set_print_color(char *color, mode_t mode)
{
	if (S_ISREG(mode))
		strncpy(color, WHITE, COLLEN);
	if (S_ISDIR(mode))
		strncpy(color, BLUE, COLLEN);
	if (S_ISCHR(mode))
		strncpy(color, LIGHT_GREEN, COLLEN);
	if (S_ISBLK(mode))
		strncpy(color, YELLOW, COLLEN);
	if (S_ISFIFO(mode))
		strncpy(color, LIGHT_BLUE, COLLEN);
	if (S_ISLNK(mode))
		strncpy(color, FLASH, COLLEN);
	if (S_ISSOCK(mode))
		strncpy(color, BLUE, COLLEN);
	if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
		if (S_ISREG(mode))
			strncpy(color, GREEN, COLLEN);
		if (S_ISUID & mode)
			strncpy(color, RED, COLLEN);
		if (S_ISGID & mode)
			strncpy(color, YELLOW, COLLEN);
	}
}

ssize_t get_last_modified_time(char *line, time_t tm)
{
	char buffer[32];
	ssize_t loffset = 0;

	memset(buffer, '\0', sizeof(buffer));
	snprintf(buffer, 13, "%s", 4 + ctime(&tm));
	loffset = sprintf(line, "%s ", buffer);

	return loffset;
}

ssize_t get_last_accessed_time(char *line, time_t tm)
{
	char buffer[32];
	ssize_t loffset = 0;

	memset(buffer, '\0', sizeof(buffer));
	snprintf(buffer, 13, "%s", 4 + ctime(&tm));
	loffset = sprintf(line, "%s ", buffer);

	return loffset;
}
ssize_t get_size2line(char *line, off_t size)
{
	ssize_t loffset = 0;

	loffset = sprintf(line, "%-5lu", size);

	return loffset;
}

ssize_t get_group_name2line(char *line, gid_t gid)
{
	ssize_t loffset = 0;
	struct group *grp;

	grp = getgrgid(gid);
	if (grp == NULL)
		loffset = sprintf(line, "%-8.d", gid);
	else
		loffset = sprintf(line, "%-8s", grp->gr_name);

	return loffset;
}

ssize_t get_user_name2line(char *line, uid_t uid)
{
	ssize_t loffset = 0;
	struct passwd *pwd;

	pwd = getpwuid(uid);
	if (pwd == NULL)
		loffset = sprintf(line, "%-8.d", uid);
	else
		loffset = sprintf(line, "%-8s", pwd->pw_name);

	return loffset;
}

ssize_t get_links2line(char *line, struct stat *st)
{
	ssize_t loffset = 0;
	nlink_t links = 0;

	links = st->st_nlink;
	loffset = sprintf(line, "%2lu ", links);
	return loffset;
}

ssize_t get_mode2line(char *line, mode_t mode)
{
	ssize_t loffset = 0;
	char tmpline[MODELEN] = {'\0'};

	set_typemode2line(tmpline+0, mode);

	set_mode2line(tmpline+1, S_IRUSR, 'r', mode);
	set_mode2line(tmpline+2, S_IWUSR, 'w', mode);
	set_specialbits2line(tmpline+3, mode, 0x1);

	set_mode2line(tmpline+4, S_IRGRP, 'r', mode);
	set_mode2line(tmpline+5, S_IWGRP, 'w', mode);
	set_specialbits2line(tmpline+6, mode, 0x2);

	set_mode2line(tmpline+7, S_IROTH, 'r', mode);
	set_mode2line(tmpline+8, S_IWOTH, 'w', mode);
	set_specialbits2line(tmpline+9, mode, 0x4);

	*(tmpline + 10) = ' ';
	strncpy(line, tmpline, strlen(tmpline));
	return strlen(tmpline);
}

int set_specialbits2line(char *line, mode_t mode, int MODE)
{
	int m = 0;
	int n = 0;

	if (MODE == 0x01) {
		m = S_IXUSR;
		n = S_ISUID;
	}
	if (MODE ==  0X02) {
		m = S_IXGRP;
		n = S_ISGID;
	}
	if (MODE == 0x04)
		m = S_IXOTH;
	
	if ((m & mode) && (n & mode)) {
		*line = 's';
		return 0;
	}
	if (!(m & mode) && (n & mode)) {
		*line = 'S';
		return 0;
	}
	if ((S_ISVTX & mode) && (n & mode)) {
		*line = 't';
		return 0;
	}
	if (!(S_ISVTX & mode) && (n & mode)) {
		*line = 'T';
		return 0;
	}

	if (m & mode)
		*line = 'x';
	else
		*line = '-';


}

void set_typemode2line(char *line, mode_t mode)
{
	if (S_ISREG(mode))
		*line = '-';
	else if (S_ISDIR(mode))
		*line = 'd';
	else if (S_ISCHR(mode))
		*line = 'c';
	else if (S_ISBLK(mode))
		*line = 'b';
	else if (S_ISFIFO(mode))
		*line = 'f';
	else if (S_ISLNK(mode))
		*line = 'l';
	else if (S_ISSOCK(mode))
		*line = 's';

	*line++;
}

void set_mode2line(char *line, int usermode, char ch,  mode_t mode)
{
	if (mode & usermode)
		*line++ = ch;
	else
		*line++ = '-';
}

/*make the void *options into a int type.*/
void construct_options(void *options, int *parameter)
{
	int flags = 0;
	char *str = NULL;
	char *ptr = (char *)options;

	while (*ptr) {
		if (*ptr == 'a')
			flags |= PARA_A;
		else if (*ptr == 'i')
			flags |= PARA_I;
		else if (*ptr == 'l')
			flags |= PARA_L;
		else if (*ptr == 'R')
			flags |= PARA_R;
		else if (*ptr == 's')
			flags |= PARA_S;
		else if (*ptr == 't')
			flags |= PARA_T;
		else if (*ptr == 'u')
			flags |= PARA_U;
		else
			break;
		ptr++;
	}
	*parameter = flags;
}

/*The basic file operation errors are as follows:
 *  EACCES	permission is denied
 *  ELOOP	too many symblic links
 *  EBADF	fd is bad
 *  EFAULT	bad address
 *  ENAMETOOLONG path it too long
 *  ENOENT	a componet of path does not exist, or path is empty
 *  ENOMEM	out of memory
 *  ENOTDIR	component of path prefix of path is not a directory
 *  EOVERFLOW
 */
int errors(const char *name, int errorno)
{
	int status = 0;
	char *lserr = "ls: can't open";
	switch (errorno) {
	case 0: break;
	case EACCES:
		fprintf(stdout, "%s %s: Permission denied!\n", lserr, name);
		status = 0; break;
	case ENOENT:
		fprintf(stdout, "%s %s: Not exist!\n", lserr, name);
		status = 1; break;
	case ENOMEM:
		fprintf(stdout, "%s %s: No more memory to operate file!\n", lserr, name);
		status = 2; break;
	case ENAMETOOLONG:
		fprintf(stdout, "%s %s: File name is too long!\n", lserr, name);
		status = 3; break;
	case -1:
		fprintf(stdout, "%s %s: No such file or directory!\n", lserr, name);
		status = 4; break;
	default:
		fprintf(stdout, "%s %s: Stat failed!\n", lserr, name);
		status = 5; break;
	}

	if (status != 0)
		exit(0);

	return 0;
}
