#ifndef _FIXUP_H
#define _FIXUP_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
uint8_t is_num(char c);
uint8_t get_line(char **dest, uint64_t *char_len, FILE *file);
void process(char *path, char *content, uint64_t content_len, char *ext, uint64_t ext_len);
#endif
