#include "lib/util.h"
#include "lib/fixup.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#define CHECK(eval, msg) { \
	if ((eval)){ \
		handle_error(msg); \
	} \
} 
const char *start_class_re = "<tr class='list (even|odd)'><td class=\"list "
                             "inline_header\" colspan=\"5\" >";
const char table[] = "</table>";
const char refresh_str[] = "<meta http-equiv=\"refresh\"";

int32_t main(int32_t argc, char *argv[]){
	char *stream_str = NULL, *class_str = NULL, *fwd_str = NULL;
	char classname[] = "Q11.htm";
	char filename[] = "Q11.htm";
	int32_t filefd;
	int32_t dirfd = AT_FDCWD;
	cap_len cl = {0};
	FILE *arg_file = NULL, *create_file = NULL;
	regex_t begin_class_re;
	regmatch_t groups[1];
	regoff_t html_head, start, end, start_new_class, off = {0};
	char *dirname = NULL;
	int32_t *find_arr = NULL;
	cap_len find_cl = {0};
	//lu dir_n = 0;
	lu path_sep_used = 0;
	struct stat directory;
	if (regcomp(&begin_class_re, start_class_re, REG_EXTENDED))
		handle_error("Compiling begin_class_re");
	if (argc < 2) {
		printf("USAGE: ./reg.out [FILENAME] [CLASSFILE] [DIRNAME(default=.)]");
	}
	if (argc > 3) {
		char *path = argv[3];
		if ((dirfd = openat(AT_FDCWD, path, O_DIRECTORY | O_RDONLY)) == -1){
			mkdirat(AT_FDCWD, path, 0755);
			CHECK((dirfd = openat(AT_FDCWD, path, O_DIRECTORY | O_RDONLY)) == -1, "Error finding directory");
		}
		int32_t l = 0;
		argc -= 1;
		//dir_n = strlen(argv[--argc]);
		path_sep_used = 1;
		if (stat(argv[argc], &directory) == -1) {
			mkdir(argv[argc], 0755);
		}
	} else {
	}
	for (lu i = 1; i < argc - 1; i += 1) {
		printf("argv[i]: %s\n", argv[i]);
		CHECK(!(arg_file = fopen(argv[i], "r")), "Couldn't open file");
		read_stream(&stream_str, &cl, arg_file);
		CHECK((regexec(&begin_class_re, stream_str, ARRLEN(groups), groups, 0)), "No valid html header");
		//if (regexec(&begin_class_re, stream_str, ARRLEN(groups), groups, 0))
		//	handle_error("No valid html header");
		// little hack to get rid of refreshing
		if (find_cl.capacity <= (ARRLEN(refresh_str) - 1) * sizeof(int)) {
			find_cl.capacity = (ARRLEN(refresh_str) - 1) * sizeof(int);
			find_arr = realloc(find_arr, find_cl.capacity);
		}
		// ll refresh =
		//     find(stream_str, refresh_str, cl.length, ARRLEN(refresh_str) - 1);
		ll refresh = find_kmp(stream_str, refresh_str, find_arr, cl.length,
				ARRLEN(refresh_str) - 1);
		// 0x7A = 'z'
		for (lu i = 18; i < 25; i += 1)
			stream_str[refresh + i] = 0x7A;
		html_head = groups->rm_so;
		class_str = stream_str + html_head;

		process(dirfd, argv[argc - 1], stream_str, html_head, ".htm", 5);

		fwd_str = class_str;

		while (!regexec(&begin_class_re, fwd_str, ARRLEN(groups), groups, 0)) {
			off = fwd_str - class_str;
			start = groups->rm_so + off;
			end = groups->rm_eo + off;
			for (lu i = 0; i < 4; i += 1) {
				if (class_str[end + i] == 0x20 || i == 3) {
					strncpy(filename +i, ".htm", 5);
					//strncpy(dirname + i + dir_n + path_sep_used, ".htm", 6);
					break;
				}
				filename[i] = class_str[end + i];
				//dirname[i + dir_n + path_sep_used] = class_str[end + i];
			}
			//printf("%s\n", dirname);
			printf("filename: %s\n", filename);
			CHECK(!(filefd = openat(dirfd, filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)), "Couldn't create file");
			write(filefd, stream_str, html_head);
			//if (!(create_file = fopen(dirname, "w")))
			//	handle_error("Couldn't create file");
			//fwrite(stream_str, sizeof(char), html_head, create_file);

			// skip to the end so start..(start of next match) will include the class
			fwd_str = class_str + end;
			if (regexec(&begin_class_re, fwd_str, ARRLEN(groups), groups, 0))
				break;
			off = fwd_str - class_str;
			start_new_class = groups->rm_so + off;
			write(filefd, class_str + start, start_new_class - start);
			//fwrite(class_str + start, sizeof(char), start_new_class - start,
			//		create_file);
			fwd_str = class_str + start_new_class;
		}
		if (find_cl.capacity <= (ARRLEN(table) - 1) * sizeof(int)) {
			find_cl.capacity = (ARRLEN(table) - 1) * sizeof(int);
			find_arr = realloc(find_arr, find_cl.capacity);
		}
		ll end_in = find_kmp(class_str + start, table, find_arr, cl.length - start,
				ARRLEN(table) - 1);
		printf("%s\n", class_str + start);
		printf("end in: %lld\n", end_in);
		if (end_in > 0){
			write(filefd, class_str + start, end_in);
			break;
		}
			//fwrite(class_str + start, sizeof(char), end_in, create_file);
		//fclose(create_file);
		close(filefd);
	}
	close(dirfd);
	//free(dirname);
	free(stream_str);
	free(find_arr);
	regfree(&begin_class_re);
}
