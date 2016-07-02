#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "gaussian_coefficients.h"

double       gf[MAX_N] = {0.};
double       GF[MAX_N] = {0.};
long long    GI[MAX_N] = {0};
long long    mask[MAX_N][MAX_N] = {0};
long long    mask2[MAX_N][MAX_N] = {0};


/*

int main(int argc, char **argv){
	int n=5;
	int i=0, j=0;
	double sigma=1;

	long long norm1 = 0, norm2 = 0, norm2_appr=0;

	if(argc == 1 ) printf("Help: ./gaussian_coefficients <dim_mask> <sigma>\n\n");

	if(argc == 2 && strcmp(argv[1], "-h") == 0){
		printf("Help: ./gaussian_coefficients <dim_mask> <sigma>\n");
		return -1;
	}

	if(argc >= 2)	n = (int)fmin(MAX_N, atoi(argv[1]));

	if(argc >= 3) sigma = (double)atof(argv[2]);

	printf("Program launched with\n  n=%d\n  sigma=%lf\n\n", n, sigma);

	printf("Calculate integer approximated coefficients for gaussian filter.\n");

	for(i=0; i<n; i++){
		printf("gf[%i]=%f\n", i, gaussian_sampling(n, i, sigma));
		gf[i] = gaussian_sampling(n, i, sigma);
	}

	printf("Get coefficients as calculated with default OpenCV functions:\n");
	get_float_coeffs(n, (double*)&gf, (double*)&GF);

	for(i=0; i<n; i++){
		printf("-d: GF[%d]=%lf\n", i, GF[i]);
	}


	printf("Get integer approximated coefficients:\n");
	get_int_coeffs(n, gf, GI);
	for(i=0; i<n; i++){
		printf("-d: GI[%d]=%lld\n", i, GI[i]);
	}
	printf("Normalization factor (1D) = %lld\n", norm1);

	printf("\nGaussian filter mask:\n");
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			mask[i][j] = GI[j]*GI[i];
			norm2 += mask[i][j];
			printf("%4lld ", mask[i][j]);
		}
		printf("\n");
	}

	printf("Normalization factor (2D) = %lld - rounded %lld - rel_error = %lf\n", norm2, round_to_pow2(norm2), (norm2-round_to_pow2(norm2))/((double)norm2) );

	printf("\nRounded to pow-of-2 Gaussian filter mask:\n");
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			mask2[i][j] = round_to_pow2(mask[i][j]);
			norm2_appr += mask2[i][j];
			printf("%4lld ", mask2[i][j]);
		}
		printf("\n");
	}
	printf("Appr normalization factor (2D) = %lld - rounded %lld - rel_error = %lf\n", norm2_appr, round_to_pow2(norm2_appr), (norm2_appr-round_to_pow2(norm2_appr))/((double)norm2_appr) );

	

	printf("\n\n----------------------------------------------------\n\n");
	printf("\nErrors (mask - pow2_mask)\n");
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			printf("%4lld ", mask[i][j]-mask2[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	printf("\nSSE(mask, mask2)=%lld\n", SSE(n, mask, mask2));


	printf("\n");
	
}
*/





double gaussian_sampling(int n, int i, double sigma){
	return exp(-pow(i - (n-1)/2, 2)/(2*sigma*sigma));
}

void get_float_coeffs(int n, double *gf, double *GF){
	int i = 0;
	double alfa = 0.;
	double sum = 0.;
	for(i=0; i<n; i++)
		sum += gf[i];
	alfa = 1/sum;
	//printf("--d: sum=%lf, alfa=%lf\n", sum, alfa);
	for(i=0; i<n; i++){
		GF[i]=alfa*gf[i];
	}
	return;
}

void get_int_coeffs(int n, double *gf, long long *GI){
	int i=0;
	double alfa_d = 1/gf[0];
//	printf("-d: alfa_d=1/gf[0]=%lf\n", alfa_d);

	//long alfa_i = (long) alfa_d;
//	printf("-d: alfa_i=(int)alfa_d=%d\n", alfa_i);


	for(i=0; i<n; i++){
		//GI[i]=round(alfa_i*gf[i]);
		//printf("GI[%d]= alfa_i*gf[%d]=%d\n", i, i, GI[i]);
		GI[i]=(long long)round(alfa_d*gf[i]);
		//printf("GI_[%d]=alfa_d*gf[%d]=%lf => %d\n", i, i,  alfa_d*gf[i], GI[i]);
	}

	return;
}

long long round_to_pow2(double x){
	long k = round(x);
	//printf("k=round(x)=%d\n", k);
	long l = log2(k);
	//printf("l=log2(k)=%d\n", l);
	//printf("%d - %d\n", abs(pow(2,l) - k), abs(pow(2,l+1) - k));
	long appr = abs(pow(2,l) - k) < abs(pow(2,l+1) - k) ? pow(2,l) : pow(2, l+1);
	//printf("best pow-of-2 approximation is %d\n", appr);
	return appr;
}

// Sum of Squared Errors for two matrices
long long SSE(int dim, long long a[MAX_N][MAX_N], long long b[MAX_N][MAX_N]){
	int i,j;
	long err=0;
	for(i=0; i<dim; i++)
		for(j=0; j<dim; j++)
			err += abs(a[i][j] - b[i][j]);//, 2);
	return err;
}


void get_custom_coeff_vector(int n, double sigma, long long ** kernel, long long * normalization_factor){
	int i=0;
	long long norm = 0;
	double gf[MAX_N]={0};
	long long * vector = (long long*) calloc(n, sizeof(long long));
	for(i=0; i<n; i++)
		gf[i]=gaussian_sampling(n, i, sigma);
	
	get_int_coeffs(n, gf, vector);

	
	for(i=0; i<n; i++)
		norm += vector[i];
	//printf("norm = %lld\n", norm);

	*kernel=vector;
	*normalization_factor = norm;
}

void get_custom_coeff_matrix(int n, double sigma, long long ** kernel, long long * normalization_factor){
	int i=0,j=0;
	double norm=0;
	double gf[MAX_N] = {0};
	//printf("Allocating %dx%d long matrix - sizeof(long)=%ld\n", n, n, sizeof(long));
	
	long long * mask = (long long*) calloc(n*n,sizeof(long long));
	
	for(i=0; i<n; i++){
		gf[i] = gaussian_sampling(n, i, sigma);
	}
	
	long long GI[MAX_N] = {0};
	get_int_coeffs(n, gf, GI);
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			mask[i*n+j] = GI[j]*GI[i];
			norm += mask[i*n+j];
			//printf("%3ld ", mask[i*n+j]);
		}
		//printf("\n");
	}

	*kernel = mask;
	*normalization_factor=norm;
}

void get_custom_pow2_coeff_matrix(int n, double sigma, long long ** kernel, long long * normalization_factor){
	//printf("get_custom_pow2_coeff_matrix called.\n");
	int i=0,j=0;
	double norm=0;
	double gf[MAX_N] = {0};
	//printf("Allocating %dx%d long matrix - sizeof(long)=%ld\n", n, n, sizeof(long));
	
	long long * mask = (long long*) calloc(n*n,sizeof(long long));
	
	for(i=0; i<n; i++){
		gf[i] = gaussian_sampling(n, i, sigma);
	}

	long long GI[MAX_N] = {0};
	get_int_coeffs(n, gf, GI);
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			mask[i*n+j] = round_to_pow2(GI[j]*GI[i]);
			norm += mask[i*n+j];
			//printf("%3ld ", mask[i*n+j]);
		}
		//printf("\n");
	}

	*kernel = mask;
	*normalization_factor=norm;
}