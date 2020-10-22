#define FAST_FOURIER_1
#ifdef FAST_FOURIER_1
#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "header.h"


using namespace std;

const int N = 512; // size of a square image, power of 2


void normalize(const int N, complex* in)
{
	for (int i = 0; i < N; ++i)
	{
		in[i].real /= (float)N;
		in[i].imag /= (float)N;
	}
}


void FFT1D(const int N, complex* in, bool inverse)
{
	double angle;
	
	if (N <= 1) return;
	
	complex* even = new complex[N / 2];
	complex* odd = new complex[N / 2];
	
	for (int i = 0, j = 0; i < N; i += 2, ++j)
	{
		even[j] = in[i];
		odd[j] = in[i + 1];
	}
	
	FFT1D(N / 2, even, inverse);
	FFT1D(N / 2, odd, inverse);
	
	for (int i = 0; i < N / 2; ++i)
	{
		angle = -2.0f * (float)M_PI * (float)i / (float)N;
		if (inverse) angle *= -1;

		complex w(cos(angle), sin(angle));

		w = w * odd[i];

		in[i] = even[i] + w;
		in[N / 2 + i] = even[i] - w;
	}

	delete[] even; delete[] odd;
}


void FFT2D(const int N, complex* in, bool inverse)
{
	// process the rows
	for (int i = 0; i < N; i++)
	{
		FFT1D(N, in + i * (long long)N, inverse);
		if (inverse) normalize(N, in + i * (long long)N);
	}

	// process the columns
	complex *col = new complex[N];
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// extract column
			col[j].real = in[j * N + i].real;
			col[j].imag = in[j * N + i].imag;
		}
		
		// perform 1D FFT on this column 
		FFT1D(N, col, inverse);
		if (inverse) normalize(N, col);

		for (int j = 0; j < N; j++)
		{
			// store result back in the array
			in[j * N + i].real = col[j].real;
			in[j * N + i].imag = col[j].imag;
		}
	}

	delete[] col;
}


int main()
{
	Image imageData = readPPM("sunrise.ppm");

	cout << imageData.w << " " << imageData.h << endl;
	cout << "PROCEEDING..." << endl;

	complex* in_r = new complex[N * (long long)N];
	complex* in_g = new complex[N * (long long)N];
	complex* in_b = new complex[N * (long long)N];

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
		{
			in_r[i * N + j] = complex(imageData.pixels[i * N + j].r, 0);
			in_g[i * N + j] = complex(imageData.pixels[i * N + j].g, 0);
			in_b[i * N + j] = complex(imageData.pixels[i * N + j].b, 0);
		}
	
	cout << "EXECUTION: Forward FFT" << endl;
	FFT2D(N, in_r, false);
	FFT2D(N, in_g, false);
	FFT2D(N, in_b, false);

	cout << "EXECUTION: Creation image for FFT " << endl;
	savePPM("fourier_optical_out.ppm", N, in_r, in_g, in_b, false);

	cout << "EXECUTION: Inverse FFT" << endl;
	FFT2D(N, in_r, true);
	FFT2D(N, in_g, true);
	FFT2D(N, in_b, true);
	cout << "EXECUTION: Create image for inverse FFT" << endl;
	savePPM("fourier_out.ppm", N, in_r, in_g, in_b, true);

	delete[] in_r; delete[] in_g; delete[] in_b;
}


#endif
