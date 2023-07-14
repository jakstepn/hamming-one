#pragma once
#include <fstream>

class ArgParser
{
public:
	std::ifstream file;
	bool find_pairs;
	bool only_gpu;
	ArgParser(int argc, char** argv);
};