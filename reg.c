#include "lib/util.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *start_class_re = "<tr class='list (even|odd)'><td class=\"list "
                             "inline_header\" colspan=\"5\" >";
const char table[] = "</table>";
const char refresh_str[] = "<meta http-equiv=\"refresh\"";

int main(int argc, char *argv[]) {
  char *stream_str = NULL, *class_str = NULL, *fwd_str = NULL;
  char classname[] = "000.htm";
  cap_len cl = {0};
  FILE *arg_file = NULL, *create_file = NULL;
  regex_t begin_class_re;
  regmatch_t groups[1];
  regoff_t html_head, start, end, start_new_class, off = {0};
  char *dirname = NULL;
  int *find_arr = NULL;
  cap_len find_cl = {0};
  lu dir_n = 0;
  lu path_sep_used = 0;
  struct stat directory;
  if (regcomp(&begin_class_re, start_class_re, REG_EXTENDED))
    handle_error("Compiling begin_class_re");
  if (argc < 1) {
    printf("USAGE: ./reg.out [FILENAME] [DIRNAME(default=.)]");
  }
  if (argc > 2) {
    dir_n = strlen(argv[--argc]);
    dirname = malloc(dir_n + ARRLEN(classname) + 3);
    path_sep_used = 1;
    strncpy(dirname, argv[argc], dir_n);
		printf("dirname: %s\n", dirname);
    dirname[dir_n] = '/';
		printf("dirname: %s\n", dirname);
    if (stat(argv[argc], &directory) == -1) {
      mkdir(argv[argc], 0755);
    }
  } else {
    dirname = malloc(ARRLEN(classname));
  }
  for (lu i = 1; i < argc; i += 1) {
    if (!(arg_file = fopen(argv[i], "r")))
      handle_error("Couldn't open file");
    read_stream(&stream_str, &cl, arg_file);
    if (regexec(&begin_class_re, stream_str, ARRLEN(groups), groups, 0))
      handle_error("No valid html header");
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
    fwd_str = class_str;

    while (!regexec(&begin_class_re, fwd_str, ARRLEN(groups), groups, 0)) {
      off = fwd_str - class_str;
      start = groups->rm_so + off;
      end = groups->rm_eo + off;
      for (lu i = 0; i < 4; i += 1) {
        if (class_str[end + i] == 0x20 || i == 3) {
          strncpy(dirname + i + dir_n + path_sep_used, ".htm", 5);
          break;
        }
        dirname[i + dir_n + path_sep_used] = class_str[end + i];
      }
      printf("%s\n", dirname);
      if (!(create_file = fopen(dirname, "w")))
        handle_error("Couldn't create file");
      fwrite(stream_str, sizeof(char), html_head, create_file);

      // skip to the end so start..(start of next match) will include the class
      fwd_str = class_str + end;
      if (regexec(&begin_class_re, fwd_str, ARRLEN(groups), groups, 0))
        break;
      off = fwd_str - class_str;
      start_new_class = groups->rm_so + off;
      fwrite(class_str + start, sizeof(char), start_new_class - start,
             create_file);
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
    if (end_in > 0)
      fwrite(class_str + start, sizeof(char), end_in, create_file);
    fclose(create_file);
  }
  free(dirname);
  free(stream_str);
  free(find_arr);
  regfree(&begin_class_re);
}
