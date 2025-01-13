#include "lpf.h"
#include "lcp.h"
#include "constants.h"
#include "suffix_array.h"
#include "benchmark.h"
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void print_help() {
    printf("Usage: program_name [options]\n");
    printf("Options:\n");
    printf("  -h, --help                       Display this help message\n");
    printf("  -s, --suffix                     Calculate the suffix and lcp arrays\n");
    printf("  -v, --validate                   Validate the suffix array agains the naive method\n");
    printf("  -b, --benchmark <length> <tries> Perform a benchmark with a random string[1...<length>], <tries> times\n");
    printf("Example:\n");
    printf("  ./PATH/TO/PROGRAM/lpf -b 1000000 10\n");
    printf("  ./PATH/TO/PROGRAM/lpf -s\n");
}

void suffix_array_from_input() {
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    int str_len;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        str_len = nread-1;
        printf("Retrieved line of length %d\n", str_len);
        LOG_FUNC(fwrite, input, nread, 1, stdout);
    } else {
        exit(1);
    }  
    printf("\n");

    // Expand character string to int type this puts a limit
    int *str = malloc(sizeof(int)*(str_len+ADDITIONAL_PADDING));

    for (int i = 0; i < str_len; i++) {
        str[i] = (int)input[i];
    }
    for (int i = str_len; i < str_len+ADDITIONAL_PADDING; i++) {
        str[i] = 0;
    }
    free(input);

    int *sa = suffix_array(str, str_len);
    print_suffix_array(str, sa, str_len);

    
    int *lcp = lcp_array(str, sa, str_len);
    printf("\nLCP: ");
    for (int i = 1; i < str_len; i++) {
        printf("%d ", lcp[i]);
    }
    printf("\n");

    printf("\n");
    free(sa);
    free(str);
    free(lcp);
}
 
int main(int argc, char *argv[]) {
    int size, tries;
    const char *short_opts = "hb:sv:";
    const struct option long_opts[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"benchmark",  required_argument, NULL, 'b'},
        {"suffix", no_argument, NULL, 's'},
        {"validate",  required_argument, NULL, 'v'},
        {NULL, 0, NULL, 0} // End marker
    };
    int opt;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'b':
                size = atoi(argv[optind-1]);
                tries = atoi(argv[optind]);
                benchmark_suffix_array(size, tries);
                return 0;
            case 's':
                suffix_array_from_input();
                return 0;
            case 'v':
                size = atoi(argv[optind-1]);
                tries = atoi(argv[optind]);
                validate(size, tries);
                return 0;
            case '?':
                print_help();
                return 0;
            default:
                print_help();
                return 0;
        }
    }
    if (argc == 1) {
        print_help();
    }

    if (optind < argc) {
        printf("Unrecognized arguments: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
        return 1;
    }
    return 0;
}

