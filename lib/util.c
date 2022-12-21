#include "util.h"
#include <regex.h>
#include <stdio.h>
#include <string.h>

void read_stream(char **buff, cap_len *cap_len, FILE *stream) {
  lu used = 0;
  char c = 0;
  if (!cap_len->capacity) {
    cap_len->capacity = 1;
    *buff = malloc(cap_len->capacity * sizeof(**buff));
  }
  while ((c = fgetc(stream)) && c != EOF) {
    if (c == '\n')
      continue;
    (*buff)[used++] = c;
    if (used >= cap_len->capacity) {
      cap_len->capacity *= 2;
      *buff = realloc(*buff, cap_len->capacity * sizeof(**buff));
    }
  }
  (*buff)[used] = 0;
  cap_len->length = used;
  return;
}
void lps(const char *needle, int *arr, lu needle_n) {
  ll i = 0, j = 1;
  printf("%lld\n", j);
  arr[0] = 0;
  while (j < needle_n) {
    if (needle[i] == needle[j]) {
      i += 1;
      printf("%lld\n", j);
      arr[j] = i;

      // arr[j] = arr[j - 1] + 1;
      // i += 1;
      j += 1;
    } else if (i == 0) {
      arr[j] = 0;
      j += 1;
    } else {
      i = arr[i - 1];
    }
  }
  return;
}
ll find_kmp(const char *haystack, const char *needle, int *lps_arr,
            lu haystack_n, lu needle_n) {
  needle_n = strlen(needle);
  haystack_n = strlen(haystack);
  lps(needle, lps_arr, needle_n);
  lu h = 0, n = 0;
  while (n < needle_n) {
    if (h >= haystack_n) {
      return -1;
    }
    if (haystack[h] == needle[n]) {
      h += 1;
      n += 1;
    } else if (n == 0)
      h += 1;
    else {
      n = lps_arr[n - 1];
    }
  }
  return h - needle_n;
}
reg start_regex(regex_t *regex, char *match_str, char *orig, lu groups_n,
                regmatch_t *groups) {
  reg ret = {0};
  if ((ret.exit = regexec(regex, match_str, groups_n, groups, 0))) {
    return ret;
  }
  ret.off = match_str - orig;
  ret.start = groups->rm_so + ret.off;
  ret.end = groups->rm_eo + ret.off;
  ret.len = ret.start - ret.end;
  return ret;
}
ll find(const char *haystack, const char *needle, lu haystack_n, lu needle_n) {
  lu hay_p = 0, needle_p = 0, next_p = 0;
  while (needle_p < needle_n) {
    if (hay_p >= haystack_n) {
      return -1;
    }
    if (haystack[hay_p] == needle[needle_p]) {
      needle_p += 1;
      hay_p += 1;
    } else {
      needle_p = 0;
      hay_p = ++next_p;
    }
  }
  return next_p;
}
