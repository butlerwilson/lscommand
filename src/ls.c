/*
 * =========================================================================
 *
 *       Filename:  ls.c
 *
 *    Description:  the main file of ls project.
 *
 *        Version:  1.0
 *        Created:  05/19/2014 09:35:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Youngcy (), youngcy.youngcy@gmail.com
 *   Organization:  Ocreator
 *
 * =========================================================================
 */

#include "lshead.h"
#include "lsoperate.h"

static struct option long_options[] = {
	{"all", no_argument, 0, 'a'},
	{"long", no_argument, 0, 'l'},
	{"time", no_argument, 0, 't'},
	{"size", no_argument, 0, 's'},
	{"inode", no_argument, 0, 'i'},
	{"Rescive", no_argument, 0, 'R'},
	{"atime", no_argument, 0, 'u'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

const char usage[] =
	"Usage: ls [OPTIONS]... [FILE]\n\t[OPTIONS] {ailRstu}\n\t[FILE] {file | path}";


const char help[] =
	"Usage: ls [OPTION]... [FILE]..."
	"\n-a, --all                  do not ignore entries starting with ."
	"\n-i, --inode                print the index number of each file"
	"\n-l                         use a long listing format"
	"\n-s, --size                 print the allocated size of each file, in blocks"
	"\n-R, --recursive            list subdirectories recursively"
	"\n-u                         with -lt: sort by, and show, access"
	                            "timewith -l: show access time and"
				    "sort by name, otherwise: sort by access time";

int main(int argc, char **argv)
{

	int flags = 0;
	int ch = 0, i = 0;
	char options[OPTLEN] = {'\0'};
	char pathname[MAX_PATH] = {'\0'};
	const char optstring[] = ":ailRstu";

	while ((ch = getopt_long(argc, argv, optstring, long_options, 0)) != -1) {
		switch (ch) {
		case 'a': options[i] = 'a'; break;
		case 'i': options[i] = 'i';break;
		case 'l': options[i] = 'l';break;
		case 'R': options[i] = 'R';break;
		case 's': options[i] = 's';break;
		case 't': options[i] = 't';break;
		case 'u': options[i] = 'u';break;
		case 'h': printf("%s\n", help); break;
		case '?': flags = 1; break;
		}
		i++;
	}
	if (argv[optind])
		strncpy(pathname, argv[optind], MAX_PATH);
	else
		strcpy(pathname, "./");
	main_entry(pathname, MAX_PATH, (void *)options);

	if (flags) printf("Usage:%s\n", usage);
	return 0;
}
