CC = g++
CFLAGS = -Wall -Werror -fopenmp
ACC_CC = pgc++
ACC_CFLAGS = -O2 -acc -ta=nvidia -Minfo=all
LDLIBS = 
OBJECTS = main.o Decap.o DecapPlacement.o PinMap.o

all	: DecapPlacementOptimization

main.o : main.cpp
Decap.o : Decap.cpp Decap.h BGA.h
DecapPlacement.o : DecapPlacement.cpp DecapPlacement.h
Pinmap.o : Pinmap.cpp Pinmap.h

DecapPlacementOptimization : $(OBJECTS)
	$(CC) -o DecapPlacementOptimization $(CFLAGS) $(LDLIBS) $(OBJECTS)

acc : $(OBJECTS)
	$(ACC_CC) -o DecapPlacementOptimization $(ACC_CFLAGS) $(LDLIBS) $(OBJECTS)

clean:
	rm -f *.o *~ DecapPlacementOptimization