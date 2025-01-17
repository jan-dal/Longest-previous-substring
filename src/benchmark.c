#include "lpf.h"
#include "util.h"
#include "tuple.h"
#include "benchmark.h"
#include "constants.h"
#include "suffix_array.h"
#include "suffix_array_qsort.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void validate_lpf(int str_len, int tries, int asize) {
    printf("Validating lpf arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
    int *str = malloc((str_len + ADDITIONAL_PADDING) * sizeof(int));
    str = random_str(str, str_len, asize);
    int *lpf1, *lpf2;
   
    for (int i = 0; i < tries; i++) {
        lpf1 = lpf_array(str, str_len);
        lpf2 = lpf_array_naive(str, str_len);
        for (int k = 0; k < str_len; k++) {
            if (memcmp(lpf1, lpf2, str_len * sizeof(int))) {
                printf("FOUND DIFFERENT LPF ARRAYS\n");
                printf_array(str, str_len);
                print_lpf_array(str, lpf1, str_len);
                printf("\n");
                print_lpf_array(str, lpf2, str_len);
                
                free(str);            
                free(lpf1);
                free(lpf2);        
                return;
            }
        }
        free(lpf1);
        free(lpf2);
    }
    free(str);
    printf("SUCCESS!\n");
}

void validate_suffix_array(int str_len, int tries, int asize) {
    printf("Validating suffix arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
    int *sa1, *sa2;
    int *str = malloc((str_len + ADDITIONAL_PADDING) * sizeof(int));

    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len, asize);
        sa1 = suffix_array(str, str_len);
        sa2 = suffix_array_qsort(str, str_len);
        int bug = 0;
        for (int k = 0; k < str_len; k++) {
            if (sa1[k] != sa2[k]) {
                printf("BUG: Suffix array differ! %d\n", k);
                fwrite(str + sa1[k],  sizeof(int), str_len - sa1[k],stdout);
                printf("\n");
                fwrite(str + sa2[k],  sizeof(int), str_len - sa2[k],stdout);
                printf("\n");
                bug = 1; 
            }
        }
        if (bug) {
            printf_array(str, str_len);
        // print_suffix_array(str, sa1, str_len);
        // printf("\n");
        // print_suffix_array(str, sa2, str_len);
            free(sa1);
            free(sa2);
            free(str);   
            return;
        }
        free(sa1);
        free(sa2);
    }
    free(str);   
    printf("SUCCESS!\n");
}

void benchmark_lpf_array(int str_len, int tries, int asize) {
    printf("Running benchmark on lpf arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 

    int *str = malloc((str_len+ADDITIONAL_PADDING) * sizeof(int));
    double s1 = 0, s2 = 0;
    
    FILE *file;
    char *filename = "lpf_results.csv";

    file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
    }

    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len, asize);
        s1 += timeit(lpf_array, str, str_len);
        s2 += timeit(lpf_array_naive, str, str_len);
    }
    s1 /= tries;
    s2 /= tries;
    
    // Write a line to the file
    if (fprintf(file, "%d,%d,%d,%lld,%lld\n", str_len, tries, asize, (long long)(1e9 * s1), (long long)(1e9 * s2)) < 0) {
        perror("Error writing to file");
        fclose(file);
    }
    
    printf("O(n):   %lld ns per call (%f seconds)\n",(long long)(1e9 * s1), s1);
    printf("O(n^3): %lld ns per call (%f seconds)\n",(long long)(1e9 * s2), s2);
    free(str);  
    fclose(file); 
}

void benchmark_suffix_array(int str_len, int tries, int asize) {
    printf("Running benchmark on suffix arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 

    int *str = malloc((str_len+ADDITIONAL_PADDING) * sizeof(int));
    double s1 = 0, s2 = 0;
    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len, asize);
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

int *random_str(int *str, int str_len, int asize) {
    if (str == NULL) {
        str = malloc(str_len * sizeof(int));
    }
 
    srand((unsigned int)time(NULL));

    for (int i = 0; i < str_len; i++) {
        str[i] = (rand() % asize) + 97;
    }
    for (int j = str_len; j < str_len+ADDITIONAL_PADDING; j++) {
        str[j] = 0;
    }
    return str;
}
