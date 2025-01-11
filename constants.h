#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ALPHABET_SIZE 128
#define DIV 3

#define MIN_LEN 127
#define TUPLE_SIZE 3
#define ADDITIONAL_PADDING 2
#define DEBUG 0

#if DEBUG
    #define LOG_MESSAGE(...) printf(__VA_ARGS__)
    #define LOG_FUNC(func, ...) func(__VA_ARGS__)
#else
    #define LOG_MESSAGE(...)
    #define LOG_FUNC(...)
#endif

struct tuple_info {
    int *positions;
    int *positions_rev;
    int *tuple_sorting;
    int *tuple_type;
    int (*values)[TUPLE_SIZE];
    int total_blocks;
    int max_name;
};
typedef struct tuple_info tuple_info;

#endif