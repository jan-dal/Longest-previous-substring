#include "constants.h"

int *suffix_array(int *str, int str_len);
int *reverse_suffix_array(int *sa, int len);

int *merge(int *str, int*sa12r, tuple_info *tinfo0, tuple_info *tinfo12);

void print_suffix(int *str, int str_len, int pos);
void print_suffix_array(int *str, int *sa, int len);
