#include "constants.h"
#include "radix.h"
#include <stdlib.h>
#include <string.h>


int *radix_sort(unsigned char *w, int stages, int offset) {
    int len = (strlen((char *)w) + DIV - 1 - offset) / DIV;
    w = w + offset;


    int **buckets = malloc(sizeof(char*)*(stages+1));
    for (int j = 0; j <= stages; j++) {
        buckets[j] = malloc(sizeof(char)*len);
    }
    int *out = buckets[0];

    for (int i = 0; i < len; i++) {
        buckets[stages][i] = i; 
    }

    for (int i = stages-1; i >= 0; i--) {
        int count[ALPHABET_SIZE] = {0};

        for (int j = 0; j < len; j++) {
            count[w[DIV*j + i]]++;
        }

        for (int j = 1; j < ALPHABET_SIZE; j++) {
            count[j] += count[j-1];
        }

        for (int j = len - 1; j >= 0; j--) {
            int tmp = w[DIV*buckets[i+1][j] + i];
            count[tmp]--; 
            buckets[i][count[tmp]] = buckets[i+1][j]; 
        }
        free(buckets[i+1]);
    }

    for (int i = 0; i < len; i++) {
        out[i] = out[i]*DIV + offset;
    }

    free(buckets);
    return out;
}


int *merge_radix(unsigned char* w1, unsigned char* w2, int *arr1, int *arr2) {
    int l1 = (strlen((char*)w1) + DIV - 1) / DIV;
    int l2 = (strlen((char*)w2) + DIV - 1) / DIV;
    int i = 0, j = 0, k = 0;

    int *out = malloc(sizeof(int)*(l1 + l2));

    while (i < l1 && j < l2) {
        int off = 0; 
        while(w1[DIV*i + off] == w2[DIV*j + off] && off < DIV) {
            off++;
        }
        if (w1[DIV*i + off] <= w2[DIV*j + off]) {
            out[k++] = arr1[i++];
        } else {
            out[k++] = arr2[j++];
        }
    }

    while (i < l1) {
        out[k++] = arr1[i++];
    }
    while (j < l2) {
        out[k++] = arr2[j++];
    }

    return out;
}