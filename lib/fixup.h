#ifndef _FIXUP_H
#define _FIXUP_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
uint8_t is_num(char c);
uint8_t get_line(char **dest, uint64_t *char_len, FILE *file);
void process(int32_t, char *, char *, uint64_t , char *, uint64_t );
#endif
