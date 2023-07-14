#include <iostream>
#include "hamming_cpu.h"
#include "hamming_gpu.cuh"
#include "arg_parser.h"
#include "word_reader.h"
#include <chrono>

#define LOOKUP_SIZE 256

int main(int argc, char* argv[])
{
	ArgParser parser = ArgParser(argc, argv);

	auto start = std::chrono::high_resolution_clock::now();
	FileReader reader = FileReader(&parser.file);
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout << std::endl;

	std::cout << "File read time (ms): " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl << std::endl;

	int* lookup_table = new int[LOOKUP_SIZE];
	lookup_table[0] = 0;
	for (int i = 0; i < LOOKUP_SIZE; i++)
	{
		lookup_table[i] = (1 & i) + lookup_table[i / 2];
	}

	if (!parser.only_gpu)
	{
		std::cout << "Hamming CPU" << std::endl;
		main_cpu(parser.find_pairs, &reader, lookup_table);
		std::cout << std::endl;
	}

	std::cout << "Hamming GPU" << std::endl;
	main_gpu(parser.find_pairs, &reader, lookup_table);

	reader.free_vectors();
	delete[] lookup_table;

	return 0;
}