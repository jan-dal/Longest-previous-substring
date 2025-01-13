#include "suffix_array.h"
#include <stdlib.h>
#include <stdio.h>

/**
* @brief Calculates the longest common prefix table.
*
* Given the SA table calculate the longest common prefix between 
* consecutive entries in SA. 
*
* This algorithm uses the relation:
* LCP[SA^(-1)[i]] - 1 <= LCP[SA^(-1)[i+1]]
* 
* which gives O(n) time complexity.
*
* @param[in] sa Suffix array.
* @param[in] len Length of sa.
*
* @return Returns the LCP array.
**/
int *lcp_array(int *str, int *sa, int len) {
    int *sar = reverse_suffix_array(sa, len, len);
    int *lcp = malloc(len * sizeof(int));

    lcp[0] = 0;
    int k = 0;

    for (int i = 0; i < len; i++) {
        int pos = sar[i]-1;
        int ppos = sa[pos - 1];
        int m = ppos > i ? len - ppos - k : len - i - k;        

        int inc = 0;
        for (int j = 0; j < m; j++) {
            if (str[ppos + j + k] != str[i + j + k]) {
                break;
            }
            inc++;
        }
        k += inc;
        lcp[pos] = k;

        if (k > 0) {
            k--;
        }
    }

    return lcp;
}