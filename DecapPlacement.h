#ifndef DECAPPLACEMENT_H
#define DECAPPLACEMENT_H

#include "Decap.h"
#include "PinMap.h"

struct Placement{
	uint8_t placement_index;
	bool tried_all;
	uint8_t best_index;
	double distance;
	
	Placement(){
		placement_index = 0;
		tried_all = false;
		best_index = 0;
		distance = 0;
	}
};

class DecapPlacement{
public:
	unsigned short decaps_num;
	Decap * decaps;

	long counter;
	
	DecapPlacement(uint8_t numDecaps, Decap * decapsPointer);
	~DecapPlacement();

	void execute_permutations_concurrent(uint8_t decapIndex, double distance, PinMap * pinMap, Placement * tracking);
	void execute_permutations_parallel(PinMap * pinMap);
	
private:
	bool check_location(Location loc, PinMap * pinMap);
	double place(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking);
	void deplace(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking);
};

#endif // DECAPPLACEMENT_H
