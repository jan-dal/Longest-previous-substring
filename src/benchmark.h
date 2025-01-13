#ifndef BENCHMARK_H
#define BENCHMARK_H

void benchmark_suffix_array(int str_len, int tries);
double timeit(int *(*f)(int*, int), int *str, int str_len);
int *random_str(int *str, int str_len);
void validate(int str_len, int tries);

#endif