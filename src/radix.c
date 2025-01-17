#include "radix.h"
#include "constants.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void print_sorting(int (*values)[TUPLE_SIZE], int *sorting, int n) {
    printf("Sorting len: %d\n", n);
    for (int i = 0; i < n; i++) {
        int idx = sorting[i];
        printf("[%d]: %d %d %d\n", idx, values[idx][0], values[idx][1], values[idx][2]);
    }
}

void print_values(int (*values)[TUPLE_SIZE], int n) {
    printf("Sorting len: %d\n", n);
    for (int i = 0; i < n; i++) {
        int idx = i;
        printf("%d %d %d\n", values[idx][0], values[idx][1], values[idx][2]);
    }
}

/**
* @brief Radix sort for the tuple structure.
*
* @param[in,out] tinfo The tuple info structure. The tuple_sorting is set at the end.
* @param[in] stages The number of loops to perform counting sort starting from the last elements.
*
**/
int *radix_sort(tuple_info *tinfo, int stages) {
    int *prev_sorting = NULL, *sorting = NULL;
    int n = tinfo->total_blocks * TUPLE_SIZE > MIN_LEN ? tinfo->total_blocks * TUPLE_SIZE : MIN_LEN;
    int out_len = tinfo->total_blocks;
    
    for (int i = stages-1; i >= 0; i--) {
        sorting = counting_sort(tinfo->values, prev_sorting, n, out_len, i);
        free(prev_sorting);
        prev_sorting = sorting;
    }
    return sorting;
}

/**
* @brief Counting sort which performs one sorting iteration for the radix sort.
*
* @param[in] values The tuples to perform the sorting on.
* @param[in] prev_sorting The previous sorting for the tuples (on the old index). ID if this is the first round.
* @param[in] n The number of individual values (tuple count * TUPLE_SIZE).
* @param[in] out_len The output array size.
* @param[in] stage Index in the value array.
*
**/
int *counting_sort(int (*values)[TUPLE_SIZE], int *prev_sorting, int n, int out_len, int stage) {
    int *count = calloc(n, sizeof(int));
    int *sorting = calloc(out_len, sizeof(int));
    
    for (int j = 0; j < out_len; j++) {
        count[values[j][stage]]++;
    }

    for (int j = 1; j < n; j++) {
        count[j] += count[j-1];
    }

    if (prev_sorting == NULL) {
        for (int j = out_len-1; j >= 0; j--) {
            int tmp = values[j][stage];
            count[tmp]--;
            sorting[count[tmp]] = j; 
        }
    } else {
        for (int j = out_len-1; j >= 0; j--) {
            int tmp = values[prev_sorting[j]][stage];
            count[tmp]--;
            sorting[count[tmp]] = prev_sorting[j]; 
        }
    }

    free(count);
    return sorting;
}
