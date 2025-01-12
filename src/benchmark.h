#ifndef BENCHMARK_H
#define BENCHMARK_H

void benchmark(int str_len, int tries);
double timeit(int *(*f)(int*, int), int *str, int str_len);
int *random_str(int *str, int str_len);

#endif