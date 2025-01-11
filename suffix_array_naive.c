#include <stdlib.h>

int **str_ptr;
int *str_len_ptr;

int compare(const void *a, const void *b) {
    int aval = *(int*)a, bval = *(int*)b;
    int alen = *str_len_ptr - aval, blen = *str_len_ptr - bval;
    int bound = alen > blen ? blen : alen;
    int *str = *str_ptr;

    for(int i = 0; i < bound; i++) {
        if (str[aval+i] != str[bval+i]) {
            return str[aval+i] - str[bval+i];
        } 
    }
    return 0;
}

int *suffix_array_qsort(int *str, int str_len) {
    int *sa = malloc(sizeof(int) * str_len);
    for (int i = 0; i < str_len; i++) {
        sa[i] = i;
    }
    str_ptr = &str;
    str_len_ptr = &str_len;
    qsort(sa, str_len, sizeof(int), compare);
    return sa;
}