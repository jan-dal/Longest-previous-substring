#include "lpf.h"
#include "lcp.h"
#include "util.h"
#include "tuple.h"
#include "constants.h"
#include "suffix_array.h"

#include <stdlib.h>
#include <stdio.h>

adjacent *create_adjacent(int *lcp, int len) {
    adjacent *adj = malloc(len * sizeof(adjacent));

    adj[0].prev = -1;
    adj[0].prev_val = 0;
    adj[0].next = 1;
    adj[0].next_val = lcp[1];

    adj[len-1].prev = len-2;
    adj[len-1].prev_val = lcp[len-1];
    adj[len-1].next = -1;
    adj[len-1].next_val = 0;

    for (int i = 1; i < len-1; i++) {
        adj[i].prev = i-1;
        adj[i].prev_val = lcp[i];
        adj[i].next = i+1;
        adj[i].next_val = lcp[i+1];
    }

    return adj;
}

/**
* @brief Create the longest previous substring table.
*
* Given suffix array SA and its LCP array calculate the LPF table.
* 
* Definition: 
* Given a string w[1...n], LPF[i] = k such that w[i...i+k-1] 
* occurs somewhere before i that is there exists j where
* w[i...i+k-1] = w[j...j+k-1] and j < i.
*   
* @param[in] str Input text.
* @param[in] str_len Length of text.
*
* @return Returns the LPF array.
**/
int *lpf_array(int *str, int str_len) {
    int *sa = suffix_array(str, str_len);
    int *sar = reverse_array(sa, str_len);
    int *lcp = lcp_array(str, sa, sar,str_len);  
    int *lpf = calloc(str_len, sizeof(int));
    adjacent *adj = create_adjacent(lcp, str_len);

    LOG_FUNC(print_suffix_array, str, sa, str_len);

    LOG_FUNC(printf_line, sa, str_len);
    LOG_FUNC(printf_line, sar, str_len);
    LOG_FUNC(printf_line, lcp, str_len);

    // print_suffix_array(str, sa, str_len);
    // printf_array(lcp, str_len);

    int index, prev, next, prev_val, next_val;

    for (int i = str_len-1; i > 0; i--) {
        index = sar[i];
        prev = adj[index].prev;
        next = adj[index].next;
        prev_val = adj[index].prev_val;
        next_val = adj[index].next_val;

        // printf("Index: %d PREV: %d %d, NEXT: %d %d\n", index, prev, prev_val, next, next_val);

        if (prev_val < next_val) {
            lpf[i] = next_val;
        } else {
            lpf[i] = prev_val;
        }

        if (next >= 0) {
            adj[next].prev = prev;
            adj[next].prev_val = next_val > prev_val ? prev_val : next_val;
        }
        if (prev >= 0) {
            adj[prev].next = next;
            adj[prev].next_val = next > 0 ? adj[next].prev_val : 0;
        }
    }
    free(sa);
    free(sar);
    free(lcp);
    free(adj);

    return lpf;
}

// Used to validate the lpf algorithm
int *lpf_array_naive(int *str, int str_len) {
    int *lpf = calloc(str_len, sizeof(int));
    int max, pmax;

    for (int i = 0; i < str_len; i++) {
        max = 0;
        for (int j = i-1; j >= 0; j--) {
            pmax = 0;
            for (int len = 0; len < str_len - i; len++) {
                if (str[i+len] == str[j+len]) {
                    pmax++;
                } else {
                    break;
                }
            }
            max = max < pmax ? pmax : max;
        }
        lpf[i] = max;
    }

    return lpf;
}