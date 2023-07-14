all: out cuda full

out: hamming_cpu.cpp output.cpp word_reader.cpp arg_parser.cpp
	g++ -c hamming_cpu.cpp output.cpp word_reader.cpp arg_parser.cpp

cuda: main.cu hamming_gpu.cu
	nvcc -c main.cu hamming_gpu.cu

full: main.o hamming_gpu.o output.o word_reader.o
	nvcc main.o hamming_cpu.o hamming_gpu.o output.o word_reader.o arg_parser.o -o HammingOne

.PHONY: clean

clean:
	rm *.o HammingOne

