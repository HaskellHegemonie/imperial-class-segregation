#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRLEN(x) sizeof(x) / sizeof(*x)
#define handle_error(x)                                                        \
  {                                                                            \
    perror(x);                                                                 \
    exit(EXIT_FAILURE);                                                        \
  }

typedef unsigned long lu;
typedef long long ll;
typedef long l;

void read_stream(char *restrict *buff, lu *capacity, FILE *stream) {
  lu used = 0;
  char c = 0;
  if (!*capacity) {
    *capacity = 1;
    *buff = malloc(*capacity * sizeof(**buff));
  }
  while ((c = fgetc(stream)) && c != EOF) {
    if (c == '\n')
      continue;
    (*buff)[used++] = c;
    if (used >= *capacity) {
      *capacity *= 2;
      *buff = realloc(*buff, *capacity * sizeof(**buff));
    }
  }
  (*buff)[used] = 0;
  return;
}
const char *begin_class_str = "[0-9]{1,2}[a-f][^>]*";
const char *end_class_str = "<tr class='list (even|odd)'><td class=\"list "
                            "inline_header\" colspan=\"5\" >";
const char *class_header = "<tr class='list even'><td class=\"list "
                           "inline_header\" colspan=\"5\" >";
int main(int argc, char *argv[]) {
  char *stream_str, *class_str, *fwd_str = NULL;
  char classname[8] = "000.htm";
  lu capacity = 0;
  FILE *arg_file, *create_file = NULL;

  regex_t begin_class_re, end_class_re = {0};
  regmatch_t groups[1];
  regoff_t html_head, start, end, end_class, off = {0};
  if (regcomp(&begin_class_re, begin_class_str, REG_EXTENDED | REG_NEWLINE))
    handle_error("Compiling begin_class_re");
  if (regcomp(&end_class_re, end_class_str, REG_EXTENDED | REG_NEWLINE))
    handle_error("Compiling end_class_re");

  for (lu i = 1; i < argc; i += 1) {
    arg_file = fopen(argv[i], "r");
    read_stream(&stream_str, &capacity, arg_file);
    if (regexec(&end_class_re, stream_str, ARRLEN(groups), groups, 0))
      handle_error("No valid html header");
    html_head = groups->rm_so;
    class_str = stream_str + groups->rm_eo;
    for (lu i = 0; i < html_head; i += 1) {
      printf("%c", stream_str[i]);
    }
    printf("\n");
    fwd_str = class_str;
    for (lu i = 0; i < html_head; i += 1) {
      fwrite(stream_str + i, sizeof(char), 1, create_file);
    }

    while (!regexec(&begin_class_re, fwd_str, ARRLEN(groups), groups, 0)) {
      create_file = fopen(classname, "w");
      for (lu i = 0; i < html_head; i += 1) {
        fwrite(stream_str + i, sizeof(char), 1, create_file);
      }
      off = fwd_str - class_str;
      start = groups->rm_so + off;
      end = groups->rm_eo + off;
      fwd_str = class_str + end;
      classname[0] += 1;
      fwrite(class_header, sizeof(*class_header), strlen(class_header),
             create_file);
      if (regexec(&end_class_re, fwd_str, ARRLEN(groups), groups, 0))
        handle_error("executing regex");
      off = fwd_str - class_str;
      end_class = groups->rm_so + off;
      end = groups->rm_eo + off;
      printf("Begin Match:\n");
      for (lu i = start; i < end_class; i += 1) {
        fwrite(class_str + i, sizeof(char), 1, stdout);
        fwrite(class_str + i, sizeof(char), 1, create_file);
      }
      printf("End Match\n");
      fwd_str = class_str + end;
    }
    // fclose(create_file);
  }
  free(stream_str);
}
