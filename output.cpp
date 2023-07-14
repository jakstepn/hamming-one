#include "output.h"
#include <iostream>

#define MAX_PRINT 10
#define BITS_PER_BYTE 8

void print_binary(int element)
{
	int mask = 1, tmpmask;
	for (int i = (sizeof(int)*BITS_PER_BYTE)-1; i >= 0; i--)
	{
		tmpmask = mask << i;
		std::cout << ((tmpmask & element) > 0 ? 1 : 0);
	}
	std::cout << std::endl;
}

void print_console_vector(int* vectors, int* result, int vector_len, int number_of_vectors, int max_pairs_per_vector)
{
	std::cout << std::endl;
	int printed = 0, j, t;
	for (int i = 0; i < number_of_vectors; i++)
	{
		t = 0;
		while ((j = result[i * max_pairs_per_vector + t]) != -1 && t < max_pairs_per_vector)
		{
			if (printed >= MAX_PRINT) return;

			printed++;
			std::cout << "First Line: " << i << std::endl;
			for (int k = 0; k < vector_len; k++)
			{
				print_binary(vectors[k * number_of_vectors + i]);
			}
			std::cout << "Second Line: " << j << std::endl;
			for (int k = 0; k < vector_len; k++)
			{
				print_binary(vectors[k * number_of_vectors + j]);
			}
			std::cout << "XOR: " << std::endl;
			for (int k = 0; k < vector_len; k++)
			{
				print_binary(vectors[k * number_of_vectors + i] ^ vectors[k * number_of_vectors + j]);
			}
			std::cout << std::endl;
			t++;
		}
	}
}