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

clean_o:
	rm *.o

clean:
	$(RM) $(PROG)
