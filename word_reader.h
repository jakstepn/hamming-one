#pragma once
#include <string>
#include <fstream>


class FileReader {
public:
	int amount_of_vectors, vector_length;
	int vector_int_length;
	int bits_per_int;
	int* vectors;

	FileReader(std::ifstream* file);

private:
	void set_vectors(std::ifstream* file);

	void parse_into(std::string line, int index);

	int change_to_int(std::string word);

	std::tuple<int, int> get_vector_length(std::ifstream* file);

public:
	void free_vectors();
};