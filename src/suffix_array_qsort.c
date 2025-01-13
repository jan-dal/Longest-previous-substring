#include <stdlib.h>

int **str_ptr;
int *str_len_ptr;

int compare(const void *a, const void *b) {
    int pos1 = *(int*)a, pos2 = *(int*)b;
    int alen = *str_len_ptr - pos1, blen = *str_len_ptr - pos2;
    int bound = alen > blen ? blen : alen;
    int *str = *str_ptr;

    for(int i = 0; i < bound; i++) {
        if (str[pos1+i] != str[pos2+i]) {
            return str[pos1+i] - str[pos2+i];
        } 
    }
    return alen - blen;
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