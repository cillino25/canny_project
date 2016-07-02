#ifndef _GAUSSIAN_COEFF_H
#define _GAUSSIAN_COEFF_H

#define MAX_N 251

double gaussian_sampling(int n, int i, double sigma);

void get_float_coeffs(int n, double *gf, double *GF);
void get_int_coeffs(int n, double *gf, long *GI);
void get_custom_coeff_vector(int n, double sigma, long ** kernel, long * normalization_factor);
void get_custom_coeff_matrix(int n, double sigma, long ** kernel, long * normalization_factor);
void get_custom_pow2_coeff_matrix(int n, double sigma, long ** kernel, long * normalization_factor);

long round_to_pow2(double x);

long SSE(int dim, long a[MAX_N][MAX_N], long b[MAX_N][MAX_N]);

#endif