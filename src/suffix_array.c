#include "suffix_array.h"
#include "constants.h"
#include "tuple.h"
#include "radix.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <byteswap.h>

int *reorder(int *arr, int *sorting, int len) {
    int *reorder = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++) {
        reorder[i] = arr[sorting[i]];
    }
    free(arr);
    return reorder;
}

void to_bign(int (*values)[TUPLE_SIZE], int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < TUPLE_SIZE; j++) {
            values[i][j] = bswap_32(values[i][j]);
        }
    }
}

int *_create_str(int *str, int *positions, int len, int a) {
    int *str0 = malloc(len * sizeof(int));
    for (int i=0; i < len; i++) {
        str0[i] = str[positions[i] + a];
    }
    return str0;
}

/**
* @brief Compute the suffix array for a given string.
* 
* This algorithm is an implementation of the work of Juha Kärkkäinen and Peter Sanders: 
* https://link.springer.com/chapter/10.1007/3-540-45061-0_73.
* Given w[1...n], a suffix array SA[1...n] holds the indices i for w[i...n] (all suffixes of w)
* in lexicographic order. 
* We can calculate SA in O(n) time using the algorithm described in the above referenced work.
* The assumption is that w is over the alphabet [1, n]. To ensure this works even for small 
* input lengths we pad the input with the '\0' character to the length of 127 to cover the ascii alphabet.
* Additionally we always add '000' to ensure no overflow.
* 
* @param[in] str String over the ascii alphabet for which the suffix array should be calculated.
* 
* @return Returns a pointer to the suffix array.
**/
int *suffix_array(int *str, int str_len) {
    LOG_MESSAGE("Calculating suffix array for:\n"); 
    LOG_FUNC(printf_line, str, str_len);
 
    tuple_info *tinfo12 = str_to_tuples(str, str_len);
    tinfo12->tuple_sorting = radix_sort(tinfo12, TUPLE_SIZE);
    int *tuple_names = name_tuples(tinfo12);

    LOG_MESSAGE("T12 info before reordering:\n");
    LOG_FUNC(print_tuple_info, tinfo12);

    if (tinfo12->max_name != tinfo12->total_blocks) {
        int *sa12 = suffix_array(tuple_names, tinfo12->total_blocks);
        free(tinfo12->tuple_sorting);
        tinfo12->tuple_sorting = sa12;
    }
    free(tuple_names);

    tinfo12->positions = reorder(tinfo12->positions, tinfo12->tuple_sorting, tinfo12->total_blocks);
    tinfo12->tuple_type = reorder(tinfo12->tuple_type, tinfo12->tuple_sorting, tinfo12->total_blocks);
    int *sa12r = reverse_suffix_array(tinfo12->positions, tinfo12->total_blocks,str_len+ADDITIONAL_PADDING);

    tuple_info *tinfo0 = create_t0_ordered(tinfo12, str, str_len);
    int *sorting0 = counting_sort(tinfo0->values, NULL, str_len > MIN_LEN ? str_len : MIN_LEN, tinfo0->total_blocks, TUPLE_SIZE-1);
    tinfo0->tuple_sorting = sorting0;

    LOG_MESSAGE("T12 info after reordering:\n");
    LOG_FUNC(print_tuple_info, tinfo12);

    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo12, "SUFFIX ARRAY FOR SA12");
    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo0, "SUFFIX ARRAY FOR SA0");
    tinfo0->positions = reorder(tinfo0->positions, tinfo0->tuple_sorting, tinfo0->total_blocks);
    to_bign(tinfo0->values, tinfo0->total_blocks);
    to_bign(tinfo12->values, tinfo12->total_blocks);

    int *sa = merge(str, sa12r, tinfo0, tinfo12);
    free(sa12r);

    LOG_MESSAGE("Calculated suffix array:\n");
    LOG_FUNC(printf_line, sa, str_len); 

    cleanup_tinfo(tinfo0);
    cleanup_tinfo(tinfo12);

    return sa;
}

/**
* @brief Merge the array of suffixes mod 0 and the array of suffixes mod 1, mod 2.
*
* Given the suffix arrays m0 and m12 merge them to get the whole suffix array.
*
* @param[in] sa_0 The suffix array mod 0.
* @param[in] sorted_0 The sorting of sa_0.
* @param[in] sa_12 The suffix array mod 1,2.
* @param[in] sorted_12 The sorting of sa_12.
* @param[in] len_0 Length of sa_0.
* @param[in] len_12 Length of sa_12.
*
* @return Returns the reversed suffix array.
**/
int *merge(int *str, int *sa12r, tuple_info *tinfo0, tuple_info *tinfo12) {
    LOG_MESSAGE("Merging\n");
    int str_len = tinfo0->total_blocks + tinfo12->total_blocks;

    int *str0 = _create_str(str, tinfo0->positions, tinfo0->total_blocks, 0);
    int *str10 = _create_str(str, tinfo0->positions, tinfo0->total_blocks, 1);
    int *str12 = _create_str(str, tinfo12->positions, tinfo12->total_blocks, 0);
    int *str012 = _create_str(str, tinfo12->positions, tinfo12->total_blocks, 1);

    int *sa = malloc(sizeof(int) * (tinfo0->total_blocks + tinfo12->total_blocks));
    int i0 = 0, i12 = 0; // Counts position in sa_12, sa_0
    int k = 0;

    int *m02 = malloc(2 * sizeof(int));
    int *m12 = malloc(2 * sizeof(int));

    int *m03 = malloc(3 * sizeof(int));
    int *m23 = malloc(3 * sizeof(int));

    while(i0 < tinfo0->total_blocks && i12 < tinfo12->total_blocks) {
        LOG_MESSAGE("\n------------------------ i : %d, j : %d\n", i12, i0);

        int cr;
        int pos_12 = tinfo12->positions[i12];
        int pos_0  = tinfo0->positions[i0];

        LOG_MESSAGE("Comparing: \n");
        LOG_MESSAGE("%d: ", pos_0);
        LOG_FUNC(printf_line, str+pos_0, str_len-pos_0);
        LOG_MESSAGE("%d: ", pos_12);
        LOG_FUNC(printf_line, str+pos_12, str_len-pos_12);
        LOG_MESSAGE("\n");
 
        if (tinfo12->tuple_type[i12] == 1) {
            LOG_MESSAGE("Case 1\n");
            m02[0] = str0[i0];
            m02[1] = sa12r[pos_0+1];

            m12[0] = str12[i12];
            m12[1] = sa12r[pos_12+1]; // We are safe from overflowing because sa_rev has -1 at the end

            LOG_MESSAGE("m0: (%d, %d) vs m12: (%d, %d)\n", m02[0], m02[1], m12[0], m12[1]);
            cr = memcmp(m02, m12, 2 * sizeof(int));
        } 
        else {
            LOG_MESSAGE("Case 2\n");
            m03[0] = str0[i0];
            m03[1] = str10[i0];
            m03[2] = sa12r[pos_0+2];

            m23[0] = str12[i12];
            m23[1] = str012[i12];
            m23[2] = sa12r[pos_12+2];
            
            LOG_MESSAGE("m0: (%d, %d, %d) vs m12: (%d, %d, %d)\n", m03[0], m03[1], m03[2], m23[0], m23[1], m23[2]);
            cr = memcmp(m03, m23, 3 * sizeof(int));
        }
        if (cr <= 0) {
            LOG_MESSAGE("%d won\n", pos_0);
            sa[k++] = pos_0;
            i0 += 1;
        } else {
            LOG_MESSAGE("%d won\n", pos_12);
            sa[k++] = pos_12;
            i12 += 1;
        }
    }
 
    while (i12 < tinfo12->total_blocks) {
        sa[k++] = tinfo12->positions[i12++];
    }
    while (i0 < tinfo0->total_blocks) {
        sa[k++] = tinfo0->positions[i0++];
    }

    free(m02);
    free(m12);
    free(m03);
    free(m23);

    free(str0);
    free(str10);
    free(str12);
    free(str012);

    return sa;
}

/**
* @brief Create the reverse of a suffix array.
*
* Given suffix array SA create SA^-1 where SA^-1[i] = j if SA[j] = i 
*
* @param[in] suffix_array The suffix array.
* @param[in] len Length of suffix_array.
*
* @return Returns the reversed suffix array.
**/
int *reverse_suffix_array(int *positions, int elements, int len) {
    LOG_MESSAGE("Reversing suffix array\n");
    int *sar = calloc(len, sizeof(int));
    for (int i = 0; i < elements; i++) {
        sar[positions[i]] = i+1;
    }
    LOG_MESSAGE("Reversing successful\n");
    return sar;
}

void print_suffix_array(int *str, int *sa, int len) {
    for (int i = 0; i < len; i++) {
        int pos = sa[i];
        printf("%d\t", pos);
        fwrite(str + pos,  sizeof(int), len - pos,stdout);
        printf("\n");
    }
}
