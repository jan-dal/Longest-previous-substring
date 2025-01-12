#include "constants.h"

#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

int *suffix_array(int *str, int str_len);
int *reverse_suffix_array(int *tinfo, int elements, int len);

int *merge(int *str, int*sa12r, tuple_info *tinfo0, tuple_info *tinfo12);

void print_suffix(int *str, int str_len, int pos);
void print_suffix_array(int *str, int *sa, int len);

#endif