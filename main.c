#include "lpf.h"
#include "suffix_array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        printf("Retrieved line of length %zd:\n", nread-1);
        fwrite(input, nread, 1, stdout);
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

    int *sa = suffix_array(str, nread-1);
    free(sa);
    free(str);

    return 0;
}

