#ifndef REG_UTIL_H
#define REG_UTIL_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRLEN(x) sizeof(x) / sizeof(*x)

#define handle_error(x)                                                        \
  {                                                                            \
    perror(x);                                                                 \
    exit(EXIT_FAILURE);                                                        \
  }

typedef unsigned long lu;
typedef long long ll;
typedef long l;

typedef struct {
  lu capacity;
  lu length;
} cap_len;
typedef struct {
  ll start;
  ll end;
  ll len;
  ll off;
  int exit;
} reg;

ll find_kmp(const char *haystack, const char *needle, int *lps_arr,
            lu haystack_n, lu needle_n);
reg start_regex(regex_t *regex, char *match_str, char *orig, lu groups_n,
                regmatch_t *groups);
void lps(const char *needle, int *arr, lu needle_n);
void read_stream(char **, cap_len *, FILE *);
ll find(const char *haystack, const char *needle, lu haystack_n, lu needle_n);
#endif
