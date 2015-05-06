#ifndef DECAPPLACEMENT_H
#define DECAPPLACEMENT_H

#include "Decap.h"
#include "PinMap.h"

struct Placement{
	uint8_t placement_index;
	bool tried_all;
	uint8_t best_index;
	
	Placement(){
		placement_index = 0;
		tried_all = false;
		best_index = 0;
	}
};

class DecapPlacement{
public:
	uint8_t decaps_num;
	Decap * decaps;
	
	DecapPlacement(uint8_t numDecaps, Decap * decapsPointer);
	~DecapPlacement();

	void execute_permutation_sequential(uint8_t decapIndex, double distance, PinMap * pinMap, Placement * tracking);
	
private:
	bool check_location(Location loc, PinMap * pinMap);
	double place(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking);
	void deplace(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking);
};

#endif // DECAPPLACEMENT_H
