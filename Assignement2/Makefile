#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=performance

all: $(PROG).cpp
	g++ -std=c++11 -Wall -pthread $(PROG).cpp -o $(PROG)

integral: numerical-integration.cpp
	g++ -std=c++11 -Wall -pthread numerical-integration.cpp -o numint.o

integral_debug: numerical-integration.cpp
	g++ -std=c++11 -g -Wall -pthread numerical-integration.cpp -o numint.o

SoE: SoE.cpp
	g++ -std=c++11 -Wall -pthread SoE.cpp -o SoE.o

benchmark_example: benchmark_example.cpp
	g++ -std=c++11 -Wall -pthread benchmark_example.cpp -o benchmark_example.o

benchmark_debug: benchmark_example.cpp
	g++ -std=c++11 -g -Wall -pthread benchmark_example.cpp -o benchmark_example.o

clean_o:
	rm *.o

clean:
	$(RM) $(PROG)
