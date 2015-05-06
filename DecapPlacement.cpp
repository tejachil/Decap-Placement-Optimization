#include "DecapPlacement.h"
#include <iostream>
#include <cmath>

using namespace std;

DecapPlacement::DecapPlacement(uint8_t numDecaps, Decap * decapsPointer){
	decaps_num = numDecaps;
	decaps = decapsPointer;
}

DecapPlacement::~DecapPlacement(){
}

void DecapPlacement::execute_permutation_sequential(uint8_t decapIndex, double distance, PinMap * pinMap, Placement * tracking){
	static double bestDistance = decaps_num * decaps[0].row_count * decaps[0].row_count;
	
	while(true){
		if(decapIndex == decaps_num){
			if (distance < bestDistance){
				bestDistance = distance;
				
				for(int i = 0; i < decapIndex; ++i){
					cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[i].placement_index].y << ") ";
					
					tracking[i].best_index = tracking[i].placement_index;
				}
				cout << "\n\tTotal Distance " << distance << "\n";
			}
			
			/*for(int i = 0; i < decapIndex; ++i){
				cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[i].placement_index].y << ") ";
			}
			
			cout << "\n\tTotal Distance " << distance << "\n";*/
			return;
		}
		
		if(tracking[decapIndex].tried_all){
			// Clear
			tracking[decapIndex].placement_index = 0;
			tracking[decapIndex].tried_all = false;
			return;
		}
		
		while(!check_location(decaps[decapIndex].placements[tracking[decapIndex].placement_index], pinMap)){
			if(tracking[decapIndex].placement_index < decaps[decapIndex].num_placements - 1)
				++tracking[decapIndex].placement_index;
			else{
				// Clear
				tracking[decapIndex].placement_index = 0;
				tracking[decapIndex].tried_all = false;
				return;
			}
		}
		
		//cout << (int)decapIndex << "-" << (int)decaps[decapIndex].placement_index << " ";
		
		double currentDistance = place(&decaps[decapIndex], decapIndex, pinMap, tracking);
		execute_permutation_sequential(decapIndex + 1, currentDistance + distance, pinMap, tracking);
		
		//cout << (int)decapIndex << "-" << (int)decaps[decapIndex].placement_index << "\t";
		
		deplace(&decaps[decapIndex], decapIndex, pinMap, tracking);
	}
}

bool DecapPlacement::check_location(Location loc, PinMap * pinMap){
	return (pinMap->map[loc.y][loc.x] == ' ');
}

double DecapPlacement::place(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking){
//	cout << "mathx " << (int)cap.placements[cap.placement_index].x << '-' << (int)cap.associated_pin->location.x << "\n";
//	cout << "mathy " << (int)cap.placements[cap.placement_index].y << '-' << (int)cap.associated_pin->location.y << "\n";

	double dist = pow(cap->placements[tracking[decapIndex].placement_index].x - cap->associated_pin->location.x, 2) 
				+ pow(cap->placements[tracking[decapIndex].placement_index].y - cap->associated_pin->location.y, 2);	
	dist = sqrt(dist);
	/*cout << "Placing " << cap->associated_pin->name 
		<< " at " << "(" << (int)cap->placements[tracking[decapIndex].placement_index].x 
		<< "," << (int)cap->placements[tracking[decapIndex].placement_index].y 
		<< ") distance = " << dist << "\n";*/
	pinMap->map[cap->placements[tracking[decapIndex].placement_index].y][cap->placements[tracking[decapIndex].placement_index].x] = cap->associated_pin->name;
	
	return dist;
}

void DecapPlacement::deplace(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking){
	/*cout << "Deplacing " << cap->associated_pin->name 
		<< " at " << "(" << (int)cap->placements[tracking[decapIndex].placement_index].x 
		<< "," << (int)cap->placements[tracking[decapIndex].placement_index].y 
		<< ") \n";*/
	pinMap->map[cap->placements[tracking[decapIndex].placement_index].y][cap->placements[tracking[decapIndex].placement_index].x] = ' ';

	if(tracking[decapIndex].placement_index < cap->num_placements - 1)
		++tracking[decapIndex].placement_index;
	else	tracking[decapIndex].tried_all = true;
}