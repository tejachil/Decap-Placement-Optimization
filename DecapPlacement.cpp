#include "DecapPlacement.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include "omp.h"

using namespace std;

DecapPlacement::DecapPlacement(uint8_t numDecaps, Decap * decapsPointer){
	decaps_num = numDecaps;
	decaps = decapsPointer;
	bestDistance = decaps_num * decaps[0].row_count * decaps[0].row_count;
	counter = 0;
	best_pinmap = NULL;
}

DecapPlacement::~DecapPlacement(){
}

void DecapPlacement::execute_permutations_concurrent(uint8_t decapIndex, double distance, PinMap * pinMap, Placement * tracking){
	while(true){
		if(decapIndex == decaps_num){
			if (distance < bestDistance){
				bestDistance = distance;
				
				if(best_pinmap == NULL){
					best_pinmap = new PinMap();
					best_pinmap->new_copy(pinMap);
				}
				else
					best_pinmap->copy(pinMap);

				for(int i = 0; i < decapIndex; ++i){
				/*	cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[i].placement_index].y << ") ";
				*/	
					tracking[i].best_index = tracking[i].placement_index;
				}
				//cout << "\n\tTotal Distance " << distance << "\n";
			}
			counter++;
			
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
		execute_permutations_concurrent(decapIndex + 1, currentDistance + distance, pinMap, tracking);
		
		//cout << (int)decapIndex << "-" << (int)decaps[decapIndex].placement_index << "\t";
		
		deplace(&decaps[decapIndex], decapIndex, pinMap, tracking);
	}
}

bool DecapPlacement::check_location(Location loc, PinMap * pinMap){
	return (pinMap->map[loc.y][loc.x] == ' ');
}

double DecapPlacement::place(Decap * cap, uint8_t decapIndex, PinMap * pinMap, Placement * tracking){
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

void DecapPlacement::execute_permutations_parallel(PinMap * pinMap){
	counter = 0;
	bestDistance = decaps_num * decaps[0].row_count * decaps[0].row_count;
	int decapSequential = (int)(decaps_num/4);

	long numThreads = 1;
	for(int i = 0; i < decapSequential; ++i)
		numThreads *= decaps[i].num_placements;

	PinMap * pinmap = new PinMap[numThreads];
	Placement ** tracking = new Placement*[numThreads];

	for(int i = 0; i < numThreads; ++i){
		tracking[i] = new Placement[decaps_num];
	}
	int threadCount = 0;
	pinmap[0].new_copy(pinMap);

	for(int decapIndex = 0; decapIndex < decapSequential + 1; ++decapIndex){
		if(decapIndex == decapSequential){	
			/*for(int i = 0; i < decapIndex; ++i){
				cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[threadCount][i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[threadCount][i].placement_index].y << ") ";
			}
			cout << '\n';//"\n\tTotal Distance " << distance << "\n";*/

			++threadCount;
			if(threadCount < numThreads){
				//cout << (int)pinmap[threadCount].total_rows << " " << (int)pinmap[threadCount].total_columns << "\n";
				
				pinmap[threadCount].new_copy(&pinmap[threadCount-1]);
		
				for(int i = 0; i < decapSequential; ++i){
					tracking[threadCount][i] = tracking[threadCount-1][i];
				}
		

				deplace(&decaps[decapIndex - 1], decapIndex - 1, &pinmap[threadCount], tracking[threadCount]);
			}
			else{
				break;
			}

			decapIndex -= 2;
			continue;		
		}
		
		if(tracking[threadCount][decapIndex].tried_all){
			//cout << "TRIED ALL \n";
			
			if(decapIndex == 0){
				cout << "Done trying all combinations\n";
				break;
			}

			// Clear
			tracking[threadCount][decapIndex].placement_index = 0;
			tracking[threadCount][decapIndex].tried_all = false;
			deplace(&decaps[decapIndex - 1], decapIndex - 1, &pinmap[threadCount], tracking[threadCount]);

			decapIndex -= 2;
			continue;
		}
		
		bool continueFlag = false;
		bool breakFlag = false;

		while(!check_location(decaps[decapIndex].placements[tracking[threadCount][decapIndex].placement_index], &pinmap[threadCount])){
			if(tracking[threadCount][decapIndex].placement_index < decaps[decapIndex].num_placements - 1){
				++tracking[threadCount][decapIndex].placement_index;
				//cout << "incrementing placement index of " << decapIndex << "\n";
			}
			else{
				// Clear
				tracking[threadCount][decapIndex].placement_index = 0;
				tracking[threadCount][decapIndex].tried_all = false;
				if(decapIndex > 0){
					decapIndex -= 2;
					continueFlag = true;
				}
				else
					breakFlag = true;

				break;
			}
		}

		if(continueFlag)	continue;
		if(breakFlag)	break;
		
		tracking[threadCount][decapIndex].distance = place(&decaps[decapIndex], decapIndex, &pinmap[threadCount], tracking[threadCount]);
	}
	
	cout << "Concurrency is unraveled for " << threadCount << " permutations.\n";
	
	//cout << "Using OpenACC with\n";
	//cout << "#pragma acc kernels loop\n";
	//#pragma acc data copy(pinmap[0:threadCount], tracking[0:threadCount], decapSequential) create(totalDistance)
	//#pragma acc kernels loop
	//#pragma acc data present (pinmap, tracking, decapSequential, totalDistance)
	cout << "Using OpenMP with\n";
	cout << "#pragma omp parallel for\n";
	#pragma omp parallel for
	for(int i = 0; i < threadCount; ++i){
		double totalDistance = 0;
		for(int j = 0; j < decapSequential; ++j){
			totalDistance += tracking[i][j].distance;
		}
		
		execute_permutations_concurrent(decapSequential, totalDistance, &pinmap[i], tracking[i]);
	}

}

void DecapPlacement::print_best_pinmap(uint8_t rows, uint8_t columns, uint8_t decapDepth, Pin * powerPins){
	cout << "Total distance = " << bestDistance << '\n';
	cout << "The optimal decap placement pinmap:\n";
	for(int i = 0; i < best_pinmap->total_rows; i++){
		for(int j = 0; j < best_pinmap->total_columns; ++j)
			if(i >= decapDepth && i < rows+decapDepth && j >= decapDepth && j < columns+decapDepth)
				best_pinmap->map[i][j] = '.';
	}
	
	for(int i = 0; i < decaps_num; ++i){
		best_pinmap->map[powerPins[i].location.y][powerPins[i].location.x] = powerPins[i].name;
	}
	
	for(int i = 0; i < best_pinmap->total_rows; i++){
		for(int j = 0; j < best_pinmap->total_columns; ++j)
			cout << best_pinmap->map[i][j] << ' ';
		cout << '\n';
	}
}