#include "lpf.h"
#include "suffix_array.h"
#include "suffix_array_naive.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
        str[i] = (rand() % 127) + 1;
    }
    return str;
}

void benchmark(int str_len, int tries) {
    int *str = malloc(str_len * sizeof(int));
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

int *str_from_input() {
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    int str_len;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        str_len = (int)nread-1;
        printf("Retrieved line of length %d\n", str_len);
        LOG_FUNC(fwrite, input, nread, 1, stdout);
    } else {
        exit(1);
    }
    printf("\n");

    // Expand character string to int type this puts a limit
    int *str = malloc(sizeof(int)*(str_len+ADDITIONAL_PADDING));

    for (int i = 0; i < str_len; i++) {
        str[i] = (int)input[i];
    }
    for (int i = str_len; i < str_len+ADDITIONAL_PADDING; i++) {
        str[i] = 0;
    }
    free(input);

    return str;
}
 
int main() {
    // int *str = NULL;
    int str_len = 10000000;
    // str = random_str(str, str_len);
    // str = str_from_input();

    // int *sa = suffix_array(str, 11);
    // print_suffix_array(str, sa, 11);
    // printf("\n");
    // int *sa_naive = suffix_array_qsort(str, str_len);
    // print_suffix_array(str, sa, str_len);

    // if (compare_tuples(sa, sa_naive, str_len) != 0) {
    //     printf("The methods differ\n");
    // } else {
    //     printf("Got the same result\n");
    // }

    // free(sa);
    // free(sa_naive);
    // free(str);
    
    benchmark(str_len, 1);

    // int input_size = 1 << 2;
    // for (int i = 0; i < 32; i++) {
    //     printf("Input size: 2^%d\n", i+2);
    //     benchmark(input_size, 1);
    //     input_size <<= 1;
    // }

    return 0;
}

