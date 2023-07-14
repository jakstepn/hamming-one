#include "arg_parser.h"
#include <iostream>
#include <fstream>
#include <string>

void usage()
{
	std::cout << "Usage: HammingOne filename [-c] [-v]" << std::endl;
	std::exit(EXIT_FAILURE);
}

ArgParser::ArgParser(int argc, char** argv)
{
	only_gpu = true;
	find_pairs = false;
	if (argc < 2 || argc > 4)
	{
		usage();
	}

	std::string arg1;
	std::string arg2;

	if (argc >= 2)
	{
		file.open(argv[1]);
	}

	if (argc == 3)
	{
		arg1 = std::string(argv[2]);
		if (!arg1.compare("-c"))
		{
			only_gpu = false;
		}
		else if (!arg1.compare("-v"))
		{
			find_pairs = true;
		}
		else
		{
			usage();
		}
	}
	else if (argc == 4)
	{
		arg1 = std::string(argv[2]);
		arg2 = std::string(argv[3]);

		if (!arg1.compare("-c"))
		{
			only_gpu = false;
			if (!arg2.compare("-v"))
			{
				find_pairs = true;
			}
			else
			{
				usage();
			}
		}
		else
		{
			usage();
		}
	}
}