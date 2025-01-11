#include "tuple.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/**
* @brief Assign to the input tuples a new letter from the alphabet.
*
* Assuming that the list of tuples was sorted (using radix sort for example)
* we can easily assign a unique letter to a tuple by incrementing the name
* when we encounter a new tuple, which reduces the alphabet size to the
* range [1, 2/3n]
*
* @param[in] tinfo Information about the tuples (must be sorted)
*
* @return Returns a pointer to an array with new names corresponding to tuples in str.
**/
int *name_tuples(tuple_info *tinfo) {
    int *tuple_names = malloc(sizeof(int) * (tinfo->total_blocks + ADDITIONAL_PADDING));
    int *sorting = tinfo->tuple_sorting; 
    int name = 1;
    tuple_names[sorting[0]] = name;

    for (int i = 1; i < tinfo->total_blocks; i++) {
        int cmp = memcmp(tinfo->values[sorting[i-1]], tinfo->values[sorting[i]], TUPLE_SIZE * sizeof(int));
        if (cmp != 0) {
            name++;
        }
        tuple_names[sorting[i]] = name;
    }
    for (int i = tinfo->total_blocks; i < tinfo->total_blocks + ADDITIONAL_PADDING; i++) {
        tuple_names[i] = 0;
    }
    tinfo->max_name = name;
    return tuple_names;
}


/**
* @brief Creates the tuple info structure for str.
*
* @param[in] str The input string.
* @param[in] str_len String length.
*
**/
tuple_info *str_to_tuples(int *str, int str_len) {
    LOG_MESSAGE("Creating new tuples\n");
    tuple_info *tinfo = malloc(sizeof(tuple_info));
    tinfo->total_blocks = str_len - (str_len + 2)/3;

    tinfo->positions = malloc(tinfo->total_blocks * sizeof(int));   
    tinfo->tuple_type = malloc(tinfo->total_blocks * sizeof(int));
    tinfo->positions_rev = malloc((str_len + TUPLE_SIZE) * sizeof(int));
    tinfo->values = malloc(tinfo->total_blocks * sizeof(*tinfo->values));
    tinfo->tuple_sorting = NULL;

    for (int i = 0; i < str_len; i++) {
        tinfo->positions_rev[i] = -2;
    }

    int k = 0;
    for (int j = 1; j < TUPLE_SIZE; j++) {
        for (int i = 0; 3*i + j + 2 < str_len + ADDITIONAL_PADDING; i++) {
            int index = 3*i + j;
            tinfo->positions[k] = index;
            tinfo->positions_rev[index] = k; 
            tinfo->tuple_type[k] = j;
            for (int q = 0; q < TUPLE_SIZE; q++) {
                tinfo->values[k][q] = str[index+q];
            }
            k++;
        }
    }
    for (int i = str_len; i < str_len + TUPLE_SIZE; i++) {
        tinfo->positions_rev[i] = tinfo->total_blocks;
    }
    LOG_MESSAGE("Allocated %d tuples.\n", tinfo->total_blocks);
    return tinfo;
}

/**
* @brief Creates the tuple info structure but only for letters mod 0.
*
* It is convinient to reuse the tuple info structure. 
* The letters are at the least significant position in the values arrays.
* The initial sorting of the letters is determined by the sorted tuples mod 1.
*
* @param[in] tinfo12 Tuple info for the sorted tuples mod 1 and mod 2.
* @param[in] str The input string.
* @param[in] str_len String length.
*
**/
tuple_info *create_t0(tuple_info *tinfo12, int *str, int str_len) {
    LOG_MESSAGE("Creating t0\n");
    tuple_info *tinfo0 = malloc(sizeof(tuple_info));

    tinfo0->total_blocks = (str_len+2)/3;
    tinfo0->positions = malloc(tinfo0->total_blocks * sizeof(int));
    tinfo0->values = malloc(tinfo0->total_blocks * sizeof(*tinfo0->values));

    tinfo0->tuple_type = NULL;
    tinfo0->positions_rev = NULL;
    tinfo0->tuple_sorting = NULL;

    int k = 0;
    for (int i = 0; i < tinfo12->total_blocks; i++) {
        int idx = tinfo12->tuple_sorting[i];
        if (tinfo12->tuple_type[idx] == 1) {
            tinfo0->positions[k] = tinfo12->positions[idx] - 1;
            tinfo0->values[k++][TUPLE_SIZE-1] = str[tinfo12->positions[idx] - 1];
        }
    }
 
    if (str_len % 3 == 1) {
        tinfo0->positions[k] = str_len - 1;
        tinfo0->values[k][TUPLE_SIZE - 1] = str[str_len-1];
    }
    LOG_MESSAGE("t0 created\n");
    return tinfo0;
}

tuple_info *create_t0_ordered(tuple_info *tinfo12, int *str, int str_len) {
    LOG_MESSAGE("Creating t0\n");
    tuple_info *tinfo0 = malloc(sizeof(tuple_info));

    tinfo0->total_blocks = (str_len+2)/3;
    tinfo0->positions = malloc(tinfo0->total_blocks * sizeof(int));
    tinfo0->values = malloc(tinfo0->total_blocks * sizeof(*tinfo0->values));

    tinfo0->tuple_type = NULL;
    tinfo0->positions_rev = NULL;
    tinfo0->tuple_sorting = NULL;

    int k = 0;
    for (int i = 0; i < tinfo12->total_blocks; i++) {
        int pos = tinfo12->positions[i];
        if (tinfo12->tuple_type[i] == 1) {
            tinfo0->positions[k] = pos - 1;
            tinfo0->values[k++][TUPLE_SIZE-1] = str[pos - 1];
        }
    }

    if (str_len % 3 == 1) {
        tinfo0->positions[k] = str_len - 1;
        tinfo0->values[k][TUPLE_SIZE - 1] = str[str_len-1];
    }
    LOG_MESSAGE("t0 created\n");
    return tinfo0;
}

/**
* @brief Compare 2 tuples of a given length.
*
* @param[in] t1 Tuple 1.
* @param[in] t2 Tuple 2.
* @param[in] len Length of the tuples.
*
* @return Returns k where k <= 0 if t1 <= t2 else k > 0.
**/
int compare_tuples(int *t1, int *t2, int len) {
    for (int i = 0; i < len; i++) {
        if (t1[i] != t2[i]) {
            return t1[i] - t2[i];
        }
    }
    return 0;
}

void print_tuple_info(tuple_info *tinfo) {
    int *tmp = malloc(sizeof(int) * tinfo->total_blocks);
    for (int i = 0; i < tinfo->total_blocks; i++) {
        tmp[tinfo->tuple_sorting[i]] = i;
    }
    
    printf("TUPLES:\nNumber of all blocks: %d\nPOS\t|\tTYPE\t|\tSORTING\t|\tVALUES\n", tinfo->total_blocks);
    for (int i=0; i < tinfo->total_blocks; i++) {
        printf("%d\t|\t%d\t|\t%d\t|\t%d\t|\t", tinfo->positions[i], tinfo->tuple_type[i], tinfo->tuple_sorting[i], tmp[i]);

        for (int j=0; j < TUPLE_SIZE; j++) {
            int val = tinfo->values[i][j];
            printf(isprint(val) ? "%c " : "'%d' ", val);
        }
        printf("\n");
    }
    free(tmp);
}


void print_sa_from_tinfo(int *str, int str_len, tuple_info *tinfo, char *title) {
    printf("\n%s\n", title);
    for (int i = 0; i < tinfo->total_blocks; i++) {
        printf("%d ", tinfo->tuple_sorting[i]);
    } 
    printf("\n");
    for (int i = 0; i < tinfo->total_blocks; i++) {
        int pos = tinfo->positions[tinfo->tuple_sorting[i]];
        printf("%d ", pos);
        printf_line(str+pos, str_len );
        printf("\n");
    }
    printf("\n");
}  

void cleanup_tinfo(tuple_info *tinfo) {
    free(tinfo->values);
    free(tinfo->positions);
    free(tinfo->tuple_type);
    free(tinfo->tuple_sorting);
    free(tinfo->positions_rev);
    free(tinfo);
}

void printf_line(int *str, int str_len) {
    for (int i = 0; i < str_len; i++) {
        int val = str[i];
        printf(isprint(val) ? "%c" : "'%d'", val);
    }
    printf("\n");
}
