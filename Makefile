CC = g++
CFLAGS = -Wall -Werror -fopenmp
LDLIBS = 
OBJECTS = main.o Decap.o DecapPlacement.o PinMap.o

all	: DecapPlacementOptimization

main.o : main.cpp
Decap.o : Decap.cpp Decap.h BGA.h
DecapPlacement.o : DecapPlacement.cpp DecapPlacement.h
Pinmap.o : Pinmap.cpp Pinmap.h

DecapPlacementOptimization : $(OBJECTS)
	$(CC) -o DecapPlacementOptimization $(CFLAGS) $(LDLIBS) $(OBJECTS)

clean:
	rm -f *.o *~ DecapPlacementOptimization