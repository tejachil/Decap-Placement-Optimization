#include "Decap.h"
#include <bitset> 

Decap::Decap(){
	regions = 0;
}
	
Decap::~Decap(){
	delete [] placements;
}

void Decap::configure(uint8_t rows, uint8_t columns, uint8_t decapDepth, uint8_t adjacencyCount, Pin* associatedPin){
	associated_pin = associatedPin;
	row_count = rows;
	column_count = columns;
	decap_depth = decapDepth;
	adjacency_count = adjacencyCount;
	
	uint8_t thirdThreshold = row_count/3;
	if(associated_pin->location.x < 4)	regions |= WEST;
	else if(associated_pin->location.x >= (row_count-4))	regions |= EAST;
	
	thirdThreshold = column_count/3;
	if(associated_pin->location.y < 4)	regions |= NORTH;
	else if(associated_pin->location.y >= (row_count-4))	regions |= SOUTH;
	
	regions_count = __builtin_popcount(regions);
	
	if(regions_count == 0)	regions = CENTRAL;
	
	regions_count = __builtin_popcount(regions);
	
	num_placements = regions_count*(decap_depth*adjacency_count);
	
	placements = new Location[num_placements];

	uint8_t placementCount = 0;
	if(regions & WEST){
		int startRow = ((associated_pin->location.y - (adjacency_count - 1)/2) < 0) ? 0 
			: ((associated_pin->location.y - (adjacency_count - 1)/2));
		for(int i = 0; i < decap_depth; ++i){
			for(int j = startRow; j < startRow+adjacency_count; ++j){
				placements[placementCount].x = i;
				if(j > row_count - 1)	placements[placementCount].y = row_count - 1;
				else	placements[placementCount].y = j;
				++placementCount;
			}
		}
	}
	if(regions & EAST){
		int startRow = ((associated_pin->location.y - (adjacency_count - 1)/2) < 0) ? 0 
			: ((associated_pin->location.y - (adjacency_count - 1)/2));
		for(int i = column_count + decap_depth; i < column_count + 2*decap_depth; ++i){
			for(int j = startRow; j < startRow+adjacency_count; ++j){
				placements[placementCount].x = i;
				if(j > row_count - 1)	placements[placementCount].y = row_count - 1;
				else	placements[placementCount].y = j;
				++placementCount;
			}
		}
	}
	if(regions & NORTH){
		int startColumn = ((associated_pin->location.x - (adjacency_count - 1)/2) < 0) ? 0 
			: ((associated_pin->location.x - (adjacency_count - 1)/2));
		for(int i = 0; i < decap_depth; ++i){
			for(int j = startColumn; j < startColumn+adjacency_count; ++j){
				placements[placementCount].y = i;
				if(j > column_count - 1)	placements[placementCount].x = column_count - 1;
				else	placements[placementCount].x = j;
				++placementCount;
			}
		}
	}
	if(regions & SOUTH){
		int startColumn = ((associated_pin->location.x - (adjacency_count - 1)/2) < 0) ? 0 
			: ((associated_pin->location.x - (adjacency_count - 1)/2));
		for(int i = row_count+decap_depth; i < row_count + 2*decap_depth; ++i){
			for(int j = startColumn; j < startColumn+adjacency_count; ++j){
				placements[placementCount].y = i;
				if(j > column_count - 1)	placements[placementCount].x = column_count - 1;
				else	placements[placementCount].x = j;
				++placementCount;
			}
		}
	}
}
