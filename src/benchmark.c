#include "suffix_array.h"
#include "suffix_array_qsort.h"
#include "benchmark.h"
#include "tuple.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void validate(int str_len, int tries) {
    int *str = malloc((str_len+ADDITIONAL_PADDING) * sizeof(int));
    int *sa1, *sa2;
    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len);
        sa1 = suffix_array(str, str_len);
        sa2 = suffix_array_qsort(str, str_len);

        int bug = 0;
        for (int i = 0; i < str_len; i++) {
            if (sa1[i] != sa2[i]) {
                printf("BUG: Suffix array differ! %d\n", i);
                bug = 1;
            }
        }
        if (bug) {
            printf_line(str, str_len);
            print_suffix_array(str, sa1, str_len);
            printf("\n");
            print_suffix_array(str, sa2, str_len);
        }
        
        free(sa1);
        free(sa2);
        if (bug) {break;}
    }

    free(str);   
}


void benchmark_suffix_array(int str_len, int tries) {
    int *str = malloc((str_len+ADDITIONAL_PADDING) * sizeof(int));
    double s1 = 0, s2 = 0;
    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len);
        s1 += timeit(suffix_array, str, str_len);
        s2 += timeit(suffix_array_qsort, str, str_len);
    }
    s1 /= tries;
    s2 /= tries;

    printf("O(n):     %lld ns per call (%f seconds)\n",(long long)(1e9 * s1), s1);
    printf("O(nlogn): %lld ns per call (%f seconds)\n",(long long)(1e9 * s2), s2);
    free(str);   
}

double timeit(int *(*f)(int*, int), int *str, int str_len) {
    clock_t start = clock();
    int *sa = f(str, str_len);
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    free(sa);
    return seconds;
}

int *random_str(int *str, int str_len) {
    if (str == NULL) {
        str = malloc(str_len * sizeof(int));
    }

    srand((unsigned int)time(NULL));

    for (int i = 0; i < str_len; i++) {
        str[i] = (rand() % 2) + 97;
    }
    for (int j = str_len; j < str_len+ADDITIONAL_PADDING; j++) {
        str[j] = 0;
    }
    return str;
}
