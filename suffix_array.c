#include "suffix_array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
    LOG_MESSAGE("Calculating suffix array for:\n")
    LOG_FUNC(printf_line, str, str_len)

    tuple_info *tinfo12 = str_to_tuples(str, str_len);
    tinfo12->tuple_sorting = radix_sort(tinfo12, TUPLE_SIZE);
    int *tuple_names = name_tuples(tinfo12);

    LOG_FUNC(print_tuple_info, tinfo12)

    if (tinfo12->max_name != tinfo12->total_blocks) {
        int *sa12 = suffix_array(tuple_names, tinfo12->total_blocks);
        free(tinfo12->tuple_sorting);
        tinfo12->tuple_sorting = sa12;
    }
    free(tuple_names);

    int *sa12r = reverse_suffix_array(tinfo12->tuple_sorting, tinfo12->total_blocks);

    tuple_info *tinfo0 = create_t0(tinfo12, str, str_len);
    int *sorting0 = counting_sort(tinfo0->values, NULL, str_len > MIN_LEN ? str_len : MIN_LEN, tinfo0->total_blocks, TUPLE_SIZE-1);
    tinfo0->tuple_sorting = sorting0;

    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo12, "SUFFIX ARRAY FOR SA12")
    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo0, "SUFFIX ARRAY FOR SA0")

    int *sa = merge(str, sa12r, tinfo0, tinfo12);
    free(sa12r);

    LOG_MESSAGE("Calculated suffix array:\n");
    LOG_FUNC(printf_line, sa, str_len)

    cleanup_tinfo(tinfo0);
    cleanup_tinfo(tinfo12);

    return sa;
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
        // printf("n = %d out_len = %d Invalid?: ", n, out_len);
        // printf_line(values[j], TUPLE_SIZE);
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
        int cmp = compare(tinfo->values[sorting[i-1]], tinfo->values[sorting[i]], TUPLE_SIZE);
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
* @brief Create the reverse of a suffix array.
*
* Given suffix array SA create SA^-1 where SA^-1[i] = j if SA[j] = i 
*
* @param[in] suffix_array The suffix array.
* @param[in] len Length of suffix_array.
*
* @return Returns the reversed suffix array.
**/
int *reverse_suffix_array(int *sa, int len) {
    LOG_MESSAGE("Reversing suffix array\n")
    int *sar = malloc(sizeof(int) * (len + TUPLE_SIZE));
    for (int i = 0; i < len; i++) {
        sar[sa[i]] = i;
    }
    for (int i = len; i < len + TUPLE_SIZE; i++) {
        sar[i] = -1;
    }
    LOG_MESSAGE("Reversing successful\n")
    return sar;
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
    int *sa = malloc(sizeof(int) * (tinfo0->total_blocks + tinfo12->total_blocks));
    int i0 = 0, i12 = 0; // Counts position in sa_12, sa_0
    int k = 0;

    int *m02 = malloc(2*sizeof(int));
    int *m12 = malloc(2*sizeof(int));

    int *m03 = malloc(3*sizeof(int));
    int *m23 = malloc(3*sizeof(int));


    while(i0 < tinfo0->total_blocks && i12 < tinfo12->total_blocks) {
        LOG_MESSAGE("\n------------------------ i : %d, j : %d\n", i12, i0)

        int cr;
        int pos_12 = tinfo12->positions[tinfo12->tuple_sorting[i12]];
        int pos_0  = tinfo0->positions[tinfo0->tuple_sorting[i0]];

        LOG_MESSAGE("Comparing: \n")
        LOG_MESSAGE("%d: ", pos_0)
        LOG_FUNC(printf_line, str+pos_0, tinfo0->total_blocks + tinfo12->total_blocks)
        LOG_MESSAGE(" and ")
        LOG_MESSAGE("%d: ", pos_12)
        LOG_FUNC(printf_line, str+pos_12, tinfo0->total_blocks + tinfo12->total_blocks)
        LOG_MESSAGE("\n")      

        if (tinfo12->tuple_type[tinfo12->tuple_sorting[i12]] == 1) {
            LOG_MESSAGE("Case 1\n");
            m02[0] = str[pos_0];
            m02[1] = sa12r[tinfo12->positions_rev[pos_0 + 1]];

            m12[0] = str[pos_12];
            m12[1] = sa12r[tinfo12->positions_rev[pos_12 + 1]]; // We are safe from overflowing because sa_rev has -1 at the end

            LOG_MESSAGE("m0: (%d, %d) vs m12: (%d, %d)\n", m02[0], m02[1], m12[0], m12[1]);
            cr = compare(m02, m12, 2);
        } 
        else {
            LOG_MESSAGE("Case 2\n");
            m03[0] = str[pos_0];
            m03[1] = str[pos_0 + 1];
            m03[2] = sa12r[tinfo12->positions_rev[pos_0 + 2]];

            m23[0] = str[pos_12];
            m23[1] = str[pos_12 + 1];
            m23[2] = sa12r[tinfo12->positions_rev[pos_12 + 2]];
            
            LOG_MESSAGE("m0: (%d, %d, %d) vs m12: (%d, %d, %d)\n", m03[0], m03[1], m03[2], m23[0], m23[1], m23[2]);
            cr = compare(m03, m23, 3);
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
        sa[k++] = tinfo12->positions[tinfo12->tuple_sorting[i12++]];
    }
    while (i0 < tinfo0->total_blocks) {
        sa[k++] = tinfo0->positions[tinfo0->tuple_sorting[i0++]];
    }

    free(m02);
    free(m12);
    free(m03);
    free(m23);

    return sa;
}

/**
* @brief Creates the tuple info structure for str.
*
* @param[in] str The input string.
* @param[in] str_len String length.
*
**/
tuple_info *str_to_tuples(int *str, int str_len) {
    LOG_MESSAGE("Creating new tuples\n")
    tuple_info *tinfo = malloc(sizeof(tuple_info));
    tinfo->total_blocks = str_len - (str_len + 2)/3;

    tinfo->positions = malloc(tinfo->total_blocks * sizeof(int));   
    tinfo->tuple_type = malloc(tinfo->total_blocks * sizeof(int));
    tinfo->positions_rev = malloc((str_len + TUPLE_SIZE) * sizeof(int));
    tinfo->values = malloc(tinfo->total_blocks * sizeof(*tinfo->values));

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
    LOG_MESSAGE("Allocated %d tuples.\n", tinfo->total_blocks)
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
    LOG_MESSAGE("Creating t0\n")
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
    LOG_MESSAGE("t0 created\n")
    return tinfo0;
}

/**
* @brief Compare 2 tuples of a given length.
*
* @param[in] t1 Tuple 1.
* @param[in] t2 Tuple 2.
* @param[in] len Length of the tuples.
*
* @return Returns -1 if t1 < t2, 0 if t1 == t2, 1 if t1 > t2.
**/
int compare(int *t1, int *t2, int len) {
    for (int i = 0; i < len; i++) {
        if (t1[i] < t2[i]) {
            return -1;
        } 
        if (t1[i] > t2[i]) {
            return 1;
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

void print_suffix_array(int *str, int *sa, int len) {
    for (int i = 0; i < len; i++) {
        int pos = sa[i];
        printf("%d\t", pos);
        fwrite(str + pos,  sizeof(int), len - pos,stdout);
        printf("\n");
    }
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

void printf_line(int *str, int str_len) {
    for (int i = 0; i < str_len; i++) {
        int val = str[i];
        printf(isprint(val) ? "%c" : "'%d'", val);
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