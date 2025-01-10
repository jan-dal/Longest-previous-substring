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

int *suffix_array(int *str, int str_len);
int *reverse_suffix_array(int *sa, int len);

int *merge(int *str, int*sa12r, tuple_info *tinfo0, tuple_info *tinfo12);

int *name_tuples(tuple_info *tinfo);
int *radix_sort(tuple_info *tinfo, int stages);
int *counting_sort(int (*values)[TUPLE_SIZE], int *prev_sorting, int n, int out_len, int stage);

void cleanup_tinfo(tuple_info *tinfo);
int compare(int *t1, int *t2, int len);
void printf_line(int *str, int str_len);
void print_tuple_info(tuple_info *tinfo);
tuple_info *str_to_tuples(int *str, int str_len);
void print_suffix(int *str, int str_len, int pos);
tuple_info *create_t0(tuple_info *tinfo12, int *str, int str_len);
void print_sa_from_tinfo(int *str, int str_len, tuple_info *tinfo, char *title);
void print_suffix_array(int *str, int *sa, int len);
