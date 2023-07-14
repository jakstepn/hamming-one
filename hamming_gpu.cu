#include <iostream>
#include "output.h"
#include "word_reader.h"
#include "hamming_gpu.cuh"
#include <algorithm>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

// unsigned long long 64 bits
#define LOOKUP_SIZE 256
#define MAX_PAIRS 10

#define check_err(ans)                                                                              \
        if (ans != cudaSuccess)																		\
        {																							\
            fprintf(stderr, "Assert: %s %s %d\n", cudaGetErrorString(ans), __FILE__, __LINE__);		\
            exit(ans);																			\
        }																							\

__device__ unsigned long long distance_between_fragments(int fragment1, int fragment2, int* bit_count)
{
	// Xor of two parts of a vector
	int to_check = fragment1 ^ fragment2;
	return static_cast<unsigned long long>(bit_count[to_check & 0xff] + bit_count[(to_check >> 8) & 0xff] +
		bit_count[(to_check >> 16) & 0xff] + bit_count[(to_check >> 24) & 0xff]);
}

__device__ int word_elem_index(int iteration, int lines_amount, int id)
{
	// Fragment index inside a vector array
	return iteration * lines_amount + id;
}

__global__ void analyze_lines(int* vectors, int vector_len, int lines_amount,
	int* pairs_vec, unsigned long long* amount_of_pairs, bool find_pairs,
	int* bit_count)
{
	// Global thread id
	int gid = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned long long distance = 0;
	int elem_index, next_elem_index, pair_count = 0;

	if (gid < lines_amount)
	{
		// For every other word
		for (int j = gid + 1; j < lines_amount; j++)
		{
			// For every int
			for (int i = 0; i < vector_len; i++)
			{
				elem_index = word_elem_index(i, lines_amount, gid);
				next_elem_index = word_elem_index(i, lines_amount, j);
				distance += distance_between_fragments(vectors[elem_index], vectors[next_elem_index], bit_count);
			}
			if (distance == 1)
			{
				pair_count++;
				if (find_pairs && pair_count <= MAX_PAIRS)
				{
					pairs_vec[gid * MAX_PAIRS + pair_count - 1] = j;
				}
			}
			distance = 0;
		}
		atomicAdd(amount_of_pairs, pair_count);
	}

	__syncthreads();
}

void main_gpu(bool find_pairs, FileReader* reader, int* lookup_table)
{
	int number_of_vectors = reader->amount_of_vectors,
		vector_length = reader->vector_int_length;

	unsigned long long* amount_of_pairs = new unsigned long long(0);

	// Array for storing pairs
	int* res = new int[number_of_vectors * MAX_PAIRS];
	try 
	{
		std::fill(res, res + number_of_vectors * MAX_PAIRS, -1);
	}
	catch (const std::bad_alloc& e) {
		fprintf(stderr, "%s", e.what());
	}

	int* d_vectors;
	unsigned long long* d_amount_of_pairs;
	int* d_lookup_table;
	int* d_res;

	int num_threads = 1024, num_blocks;
	num_blocks = (number_of_vectors + num_threads - 1) / num_threads;

	// Timers
	float time;
	cudaEvent_t start, stop;

	check_err(cudaEventCreate(&start));
	check_err(cudaEventCreate(&stop));

	// Device memory allocation
	check_err(cudaMalloc(&d_vectors, sizeof(int) * vector_length * number_of_vectors));
	check_err(cudaMalloc(&d_res, sizeof(int) * number_of_vectors * MAX_PAIRS));
	check_err(cudaMalloc(&d_amount_of_pairs, sizeof(unsigned long long)));
	check_err(cudaMalloc(&d_lookup_table, sizeof(int) * LOOKUP_SIZE));

	check_err(cudaMemcpy(d_res, res, sizeof(int) * number_of_vectors * MAX_PAIRS, cudaMemcpyHostToDevice));
	check_err(cudaMemcpy(d_vectors, reader->vectors, sizeof(int) * number_of_vectors * vector_length, cudaMemcpyHostToDevice));
	check_err(cudaMemcpy(d_amount_of_pairs, amount_of_pairs, sizeof(unsigned long long), cudaMemcpyHostToDevice));
	check_err(cudaMemcpy(d_lookup_table, lookup_table, sizeof(int) * LOOKUP_SIZE, cudaMemcpyHostToDevice));

	check_err(cudaEventRecord(start, 0));

	analyze_lines<<<num_blocks, num_threads>>>(d_vectors, vector_length, number_of_vectors,
		d_res, d_amount_of_pairs, find_pairs, d_lookup_table);
	check_err(cudaDeviceSynchronize());

	check_err(cudaEventRecord(stop, 0));
	check_err(cudaEventSynchronize(stop));
	check_err(cudaEventElapsedTime(&time, start, stop));

	check_err(cudaEventDestroy(start));
	check_err(cudaEventDestroy(stop));

	check_err(cudaMemcpy(amount_of_pairs, d_amount_of_pairs, sizeof(unsigned long long), cudaMemcpyDeviceToHost));

	std::cout << "Pairs: " << *amount_of_pairs << std::endl;
	std::cout << "Calculation time (ms): " << time << std::endl;

	check_err(cudaMemcpy(res, d_res, sizeof(int) * number_of_vectors * MAX_PAIRS, cudaMemcpyDeviceToHost));

	if (find_pairs)
		print_console_vector(reader->vectors, res, vector_length, number_of_vectors, MAX_PAIRS);

	check_err(cudaFree(d_vectors));
	check_err(cudaFree(d_res));
	check_err(cudaFree(d_amount_of_pairs));
	check_err(cudaFree(d_lookup_table));

	delete[] res;
	delete amount_of_pairs;
}