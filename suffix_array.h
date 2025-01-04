#define MIN_LEN 127
#define TUPLE_SIZE 3
#define ADDITIONAL_PADDING 2

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

int *radix_sort(tuple_info *tinfo, int stages);
int *counting_sort(int (*values)[TUPLE_SIZE], int *prev_sorting, int n, int out_len, int stage);
int *name_tuples(tuple_info *tinfo);

int compare(int *t1, int *t2, int len);
tuple_info *str_to_tuples(int *str, int str_len);
tuple_info *create_t0(tuple_info *tinfo12, int *str, int str_len);
void cleanup_tinfo(tuple_info *tinfo);
void print_tuple_info(tuple_info *tinfo);
void print_suffix(int *str, int str_len, int pos);
void print_sa_from_tinfo(int *str, int str_len, tuple_info *tinfo, char *title);
