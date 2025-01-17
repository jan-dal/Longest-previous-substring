#ifndef BENCHMARK_H
#define BENCHMARK_H

void benchmark_suffix_array(int str_len, int tries, int asize);
void benchmark_lpf_array(int str_len, int tries, int asize);
double timeit(int *(*f)(int*, int), int *str, int str_len);
int *random_str(int *str, int str_len, int asize);
void validate_suffix_array(int str_len, int tries, int asize);
void validate_lpf(int str_len, int tries, int asize);

#endif