#include "lpf.h"
#include "suffix_array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void timeit(int *str, int str_len) {
    clock_t start = clock();
    int *sa = suffix_array(str, str_len);
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("%lld ns per call (%f seconds)\n",(long long)(1e9 * seconds), seconds);
    free(sa);
}

int main() {
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        printf("Retrieved line of length %zd\n", nread-1);
        LOG_FUNC(fwrite, input, nread, 1, stdout);
    } else {
        exit(1);
    }
    printf("\n");

    // Expand character string to int type this puts a limit
    int *str = malloc(sizeof(int)*(nread-1+ADDITIONAL_PADDING));

    for (int i = 0; i < nread-1; i++) {
        str[i] = (int)input[i];
    }
    for (int i = nread-1; i < nread-1+ADDITIONAL_PADDING; i++) {
        str[i] = 0;
    }
 
    free(input);
    timeit(str, nread-1);
    // int *sa = suffix_array(str, nread-1);
    // print_suffix_array(str, sa, nread-1);

    // free(sa);
    free(str);

    return 0;
}

