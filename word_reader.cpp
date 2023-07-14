
#include "word_reader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <cstring>
#include <string.h>

#define BITS_PER_BYTE 8
#define VALUES_SIZE 50

FileReader::FileReader(std::ifstream* file)
{
	bits_per_int = sizeof(int) * BITS_PER_BYTE;
	set_vectors(file);
}

void FileReader::set_vectors(std::ifstream* file)
{
	std::tuple<int, int> lengths = get_vector_length(file);
	amount_of_vectors = std::get<0>(lengths);
	vector_length = std::get<1>(lengths);

	vector_int_length = (vector_length + bits_per_int - 1) / bits_per_int;

	std::string line;
	vectors = new int[amount_of_vectors * vector_int_length];
	for (int i = 0; i < amount_of_vectors; i++)
	{
		std::getline(*file, line);
		parse_into(line, i);
	}
	std::cout << "Number of vectors: " << amount_of_vectors << std::endl;
	std::cout << "Vector length: " << vector_length << std::endl;
	std::cout << "Length of a parsed " << sizeof(int) << " byte per element array: " << vector_int_length << std::endl;

	(*file).close();
}

void FileReader::parse_into(std::string line, int index)
{
	for (int j = 0; j < vector_int_length; j++)
	{
		std::string sub = line.substr(j * bits_per_int,
			std::min(bits_per_int, vector_length - j * bits_per_int));

		vectors[index + j * amount_of_vectors] = change_to_int(sub);
	}
}

int FileReader::change_to_int(std::string vector)
{
	int mask = 1, tmpmask, res = 0;
	for (int i = 0; i < vector.length(); i++)
	{
		if (vector.at(i) == '1')
		{
			tmpmask = mask << (bits_per_int - 1 - i);
			res |= tmpmask;
		}
	}
	return res;
}

std::tuple<int, int> FileReader::get_vector_length(std::ifstream* file)
{
	// Get the vector length and the number of vectors
	int first, second;
	std::string line;
	std::getline(*file, line);

	char buffer[VALUES_SIZE + 1];
	buffer[VALUES_SIZE] = '\0';
	strcpy(buffer, line.c_str());

	char* token = strtok(buffer, " ,");
	first = atoi(token);
	token = strtok(nullptr, " ,");
	second = atoi(token);

	return std::make_tuple(first, second);
}

void FileReader::free_vectors()
{
	delete[] vectors;
}
