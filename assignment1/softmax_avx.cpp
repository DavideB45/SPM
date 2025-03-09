#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <limits>      
#include <hpc_helpers.hpp>
#include <immintrin.h>
#include <avx_mathfun.h>

/* 
The following mask is a suggestion to handle
array with length not multiple of 8
The code was taken from stackoverflow
answered by robthebloke

https://stackoverflow.com/questions/73739507/how-do-you-handle-indivisible-vector-lengths-with-simd-intrinsics-array-not-a-m
*/
const __m128i temp[5] = {
	_mm_setr_epi32(0, 0, 0, 0),
	_mm_setr_epi32(-1, 0, 0, 0),
	_mm_setr_epi32(-1, -1, 0, 0),
	_mm_setr_epi32(-1, -1, -1, 0),
	_mm_setr_epi32(-1, -1, -1, -1)
};

const __m256i mask_table[8] = {
	_mm256_setr_epi32( 0,  0,  0,  0,  0,  0,  0,  0),  // cr = 0
	_mm256_setr_epi32(-1,  0,  0,  0,  0,  0,  0,  0),  // cr = 1
	_mm256_setr_epi32(-1, -1,  0,  0,  0,  0,  0,  0),  // cr = 2
	_mm256_setr_epi32(-1, -1, -1,  0,  0,  0,  0,  0),  // cr = 3
	_mm256_setr_epi32(-1, -1, -1, -1,  0,  0,  0,  0),  // cr = 4
	_mm256_setr_epi32(-1, -1, -1, -1, -1,  0,  0,  0),  // cr = 5 (same as 4)
	_mm256_setr_epi32(-1, -1, -1, -1, -1, -1,  0,  0),  // cr = 6 (same as 4)
	_mm256_setr_epi32(-1, -1, -1, -1, -1, -1, -1,  0)   // cr = 7 (same as 4)
};

/*
The following two functions were inspired from the slide of the curse,
the only changes were the substitution of 'add' with 'sum'
Parallel and Distribted System by prof. Massimo Torquati
*/
// horizontal max using SSE3 (4 floats)
inline float hmax_sse3(__m128 v) {
	__m128 shuf = _mm_movehdup_ps(v);
	__m128 maxs = _mm_max_ps(v, shuf);
	shuf = _mm_movehl_ps(shuf, maxs);
	maxs = _mm_max_ss(maxs, shuf);
	// extract the lowest 32-bit float
	return _mm_cvtss_f32(maxs);
}
inline float hmax_avx(__m256 v) {
	__m128 lo = _mm256_castps256_ps128(v); // cast the lower 128-bits of v (no move)
	__m128 hi = _mm256_extractf128_ps(v, 1); // moves the higher 128-bits of v
	lo = _mm_max_ps(lo, hi); // max the four float values of lo and hi
	return hmax_sse3(lo); // horizontal max of 4 floats
}

/*
The following code is copied from the slides of SPM course
*/
// horizontal sum using SSE3 (4 floats)
inline float hsum_sse3(__m128 v) {
	__m128 shuf = _mm_movehdup_ps(v);
	__m128 maxs = _mm_add_ps(v, shuf);
	shuf = _mm_movehl_ps(shuf, maxs);
	maxs = _mm_add_ss(maxs, shuf); // extract the lowest 32-bit float
	return _mm_cvtss_f32(maxs);
}
inline float hsum_avx(__m256 v) {
	__m128 lo = _mm256_castps256_ps128(v); // cast the lower 128-bits of v (no move)
	__m128 hi = _mm256_extractf128_ps(v, 1); // moves the higher 128-bits of v
	lo = _mm_add_ps(lo, hi); // adds the four float values of lo and hi
	return hsum_sse3(lo); // horizontal sum of 4 floats
}

void softmax_avx(const float *input, float *output, size_t K) {
	// if they are at least 8 else it does
	size_t K2 = K-7;
	//__m256 max_val = _mm256_loadu_ps(input);
	__m256 max_val = _mm256_loadu_ps(input); 
	// allineare la memoria
	__m256 curr_val;
	for (size_t i = 8; i < K2; i += 8) {
		curr_val = _mm256_loadu_ps(input + i);
		max_val = _mm256_max_ps(curr_val, max_val);
	}
	max_val = _mm256_max_ps(_mm256_loadu_ps(input + K - 8), max_val); // process the last 8 elements
	max_val = _mm256_set1_ps(hmax_avx(max_val)); // create a vector of max for subtraction

	// computes all exponentials with the shift of max_val and the total sum
    __m256 sum_vector = _mm256_setzero_ps();
    for (size_t i = 0; i < K2; i += 8) {
		curr_val = _mm256_loadu_ps(input + i);
		curr_val = exp256_ps(_mm256_sub_ps(curr_val, max_val));
        _mm256_storeu_ps(output + i, curr_val);
        sum_vector = _mm256_add_ps(sum_vector, curr_val);
    }
	const unsigned cr = K & 0x7U; // K % 8 made in a (probably) faster way
	// __m256i mask;
	// if(cr >= 4) { 
    //     mask = _mm256_set_m128i(temp[cr&3], temp[4]);
    // } else {
    //     mask = _mm256_set_m128i(temp[0], temp[cr]);
    // }

	__m256i mask = mask_table[cr & 7];  // Fast lookup, avoiding branching

	curr_val = _mm256_maskload_ps(input + K - cr, mask);
	curr_val = exp256_ps(_mm256_sub_ps(curr_val, max_val));
	_mm256_maskstore_ps(output + K - cr, mask, curr_val);
	curr_val = _mm256_maskload_ps(output + K - cr, mask);
	sum_vector = _mm256_add_ps(sum_vector, curr_val);

    // normalize by dividing for the total sum
	__m256 sum_vec = _mm256_set1_ps(hsum_avx(sum_vector));
    for (size_t i = 0; i < K2; i += 8) {
		_mm256_storeu_ps(output + i, _mm256_div_ps(_mm256_loadu_ps(output + i),sum_vec));
    }
	_mm256_maskstore_ps(output + K - cr, mask, _mm256_div_ps(_mm256_maskload_ps(output + K - cr, mask), sum_vec));
}


std::vector<float> generate_random_input(size_t K, float min = -1.0f, float max = 1.0f) {
//float* generate_random_input(size_t K, float min = -1.0f, float max = 1.0f) {
	//float* input = (float*)_mm_malloc(K*sizeof(float)*8, 32);
	std::vector<float> input(K);
	//std::random_device rd;
    //std::mt19937 gen(rd());
	std::mt19937 gen(5489); // fixed seed for reproducible results
    std::uniform_real_distribution<float> dis(min, max);
    for (size_t i = 0; i < K; ++i) {
        input[i] = dis(gen);
    }
    return input;
}

void printResult(float* v, size_t K) {
	for(size_t i=0; i<K; ++i) {
		std::fprintf(stderr, "%f\n",v[i]);
	}
}


int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::printf("use: %s K [1]\n", argv[0]);
		return 0;		
	}
	size_t K=0;
	if (argc >= 2) {
		K = std::stol(argv[1]);
	}
	bool print=false;
	if (argc == 3) {
		print=true;
	}
	// aligned menory
	std::vector<float> input=generate_random_input(K);
	//float* input=generate_random_input(K);
	std::vector<float> output(K);
	//float* output = (float*)_mm_malloc(K*sizeof(float)*8, 32);

	TIMERSTART(softime_avx);
	//softmax_avx(input.data(), output.data(), K);
	softmax_avx(input.data(), output.data(), K);
	TIMERSTOP(softime_avx);
	
	// print the results on the standard output
	if (print) {
		printResult(output.data(), K);
	}

	// free memory if allocated with _mm_malloc
	//_mm_free(input);
	//_mm_free(output);
	return 0;
}

