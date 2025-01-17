#ifndef LPF_H
#define LPF_H

struct adjacent {
    int prev;
    int next;
    int prev_val;
    int next_val;
};
typedef struct adjacent adjacent;

int *lpf_array(int *str, int str_len);
int *lpf_array_naive(int *str, int str_len);

#endif