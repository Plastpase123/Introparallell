#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=performance

all: $(PROG).cpp
	g++ -std=c++11 -Wall -pthread $(PROG).cpp -o $(PROG)


SoE: SoE.cpp
	g++ -std=c++11 -fopenmp -Wall -pthread SoE.cpp -o SoE.o

GoL: Game_Of_Life.c
	gcc Game_Of_Life.c -fopenmp -o Game_Of_Life

GoL_gif: Game_Of_Life.c
	gcc Game_Of_Life.c -DOUTPUT -fopenmp -o Game_Of_Life

clean_o:
	rm *.o

clean:
	$(RM) $(PROG)
