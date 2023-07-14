#include <iostream>
#include "output.h"
#include "word_reader.h"
#include "hamming_cpu.h"
#include <chrono>
#include <algorithm>

#define MAX_PAIRS 10

void analyze_lines(int* vectors, int vector_len, int number_of_vectors, int* result, bool find_pairs, int* bit_count)
{
	int distance = 0, to_check, i_val;
	unsigned long long pairs = 0, pair_counter = 0;

	unsigned long long* counter = new unsigned long long[number_of_vectors];
	try
	{
		std::fill(counter, counter + number_of_vectors, 0);
	}
	catch (const std::bad_alloc& e) {
		fprintf(stderr, "%s", e.what());
	}

	// Foreach word
	for (int i = 0; i < number_of_vectors; i++)
	{
		// For every element
		for (int k = 0; k < vector_len; k++)
		{
			i_val = vectors[i + k * number_of_vectors];
			// For every other word
			for (int j = i + 1; j < number_of_vectors; j++)
			{
				to_check = i_val ^ vectors[j + k * number_of_vectors];
				counter[j] += static_cast<unsigned long long>(bit_count[to_check & 0xff] + bit_count[(to_check >> 8) & 0xff] +
					bit_count[(to_check >> 16) & 0xff] + bit_count[(to_check >> 24) & 0xff]);
			}
		}
		for (int j = i + 1; j < number_of_vectors; j++)
		{
			if (counter[j] == 1)
			{
				pair_counter++;
				if (find_pairs && pair_counter <= MAX_PAIRS)
				{
					result[i * MAX_PAIRS + pair_counter - 1] = j;
				}
				pairs++;
			}
			counter[j] = 0;
		}
		pair_counter = 0;
	}

	delete[] counter;
	std::cout << "Pairs: " << pairs << std::endl;
}


void main_cpu(bool find_pairs, FileReader* reader, int* lookup_table)
{
	int* res = new int[reader->amount_of_vectors * MAX_PAIRS];
	auto start = std::chrono::high_resolution_clock::now();
	analyze_lines(reader->vectors, reader->vector_int_length, reader->amount_of_vectors,
		res, find_pairs, lookup_table);
	auto stop = std::chrono::high_resolution_clock::now();

	std::cout << "Calculation time (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;

	if (find_pairs)
		print_console_vector(reader->vectors, res, reader->vector_int_length, reader->amount_of_vectors, MAX_PAIRS);

	delete[] res;
}