#ifndef DECAP_H
#define DECAP_H

#include "BGA.h"

enum Region{NORTH=0x1, EAST=0x2, SOUTH=0x4, WEST=0x8, CENTRAL=0xF};

class Decap{
public:
	Location* placements;
	Pin* associated_pin;
	uint8_t row_count, column_count, decap_depth, adjacency_count, regions, regions_count, num_placements;
	
	Decap();
	~Decap();
	
	void configure(uint8_t rows, uint8_t columns, uint8_t decapDepth, uint8_t adjacencyCount, Pin* associatedPin);
};

#endif // DECAP_H
