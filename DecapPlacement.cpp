#include "DecapPlacement.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include "omp.h"
#include "openacc.h"

using namespace std;

static long counter = 0;
static double bestDistance;

DecapPlacement::DecapPlacement(uint8_t numDecaps, Decap * decapsPointer){
	decaps_num = numDecaps;
	decaps = decapsPointer;
	bestDistance = decaps_num * decaps[0].row_count * decaps[0].row_count;
}

DecapPlacement::~DecapPlacement(){
}

void DecapPlacement::execute_permutations_concurrent(uint8_t decapIndex, double distance, PinMap * pinMap, Placement * tracking){
	while(true){
		if(decapIndex == decaps_num){
			if (distance < bestDistance){
				bestDistance = distance;
				//cout << "\n";
				cout << counter << ":\n";
				for(int i = 0; i < decapIndex; ++i){
					cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[i].placement_index].y << ") ";
					
					tracking[i].best_index = tracking[i].placement_index;
				}
				cout << "\n\tTotal Distance " << distance << "\n";
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
	//pinMap->map[cap->placements[tracking[decapIndex].placement_index].y][cap->placements[tracking[decapIndex].placement_index].x] << '\n';
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

	long numThreads = 1;
	for(int i = 0; i < decaps_num/4; ++i)
		numThreads *= decaps[i].num_placements;

	PinMap * pinmap = new PinMap[numThreads];
	Placement ** tracking = new Placement*[numThreads];

	for(int i = 0; i < numThreads; ++i){
		//pinmap[i].copy(pinMap);
		tracking[i] = new Placement[decaps_num];
	}
	int threadCount = 0;
	double totalDistance = 0;
	pinmap[0].copy(pinMap);

	for(int decapIndex = 0; decapIndex < decaps_num/4 + 1; ++decapIndex){
		if(decapIndex == ((int)decaps_num/4)){	
			/*for(int i = 0; i < decapIndex; ++i){
				cout << decaps[i].associated_pin->name << '(' 
						<< (short)decaps[i].placements[tracking[i].placement_index].x << ','
						<< (short)decaps[i].placements[tracking[i].placement_index].y << ") ";
			}
			cout << "\n\tTotal Distance " << distance << "\n";*/
		
			/*totalDistance = 0;
			for(int i = 0; i < decaps_num/4; ++i){
				totalDistance += tracking[threadCount][i].distance;
			}

			// TODO: execute the sequential;
			execute_permutations_concurrent(decapIndex, totalDistance, &pinmap[threadCount], tracking[threadCount]);
*/
			++threadCount;
			if(threadCount < numThreads){
				//cout << (int)pinmap[threadCount].total_rows << " " << (int)pinmap[threadCount].total_columns << "\n";
				
				pinmap[threadCount].copy(pinMap);
		
				for(int i = 0; i < decaps_num/4; ++i){
					tracking[threadCount][i] = tracking[threadCount-1][i];
				}
		
				//cout << threadCount << ": ";
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
		
		while(!check_location(decaps[decapIndex].placements[tracking[threadCount][decapIndex].placement_index], &pinmap[threadCount])){
			if(tracking[threadCount][decapIndex].placement_index < decaps[decapIndex].num_placements - 1){
				++tracking[threadCount][decapIndex].placement_index;
				//cout << "incrementing placement index of " << decapIndex << "\n";
			}
			else{
				// Clear
				tracking[threadCount][decapIndex].placement_index = 0;
				tracking[threadCount][decapIndex].tried_all = false;

				decapIndex -= 2;
			}
		}
		
		//cout << (int)decapIndex << "-" << (int)decaps[decapIndex].placement_index << " ";
		tracking[threadCount][decapIndex].distance = place(&decaps[decapIndex], decapIndex, &pinmap[threadCount], tracking[threadCount]);

		/*execute_permutations_concurrent(decapIndex + 1, currentDistance + distance, &pinMap[threadCount], tracking[threadCount]);
		
		//cout << (int)decapIndex << "-" << (int)decaps[decapIndex].placement_index << "\t";
		
		deplace(&decaps[decapIndex], decapIndex, &pinMap[threadCount], tracking[threadCount]);*/

	}


	clock_t time_start = clock();
	
	cout << "The CURRENT THREAD COUNT IS " << threadCount << '\n';
	uint8_t decapIndex = (int)(decaps_num/4);
	//omp_set_dynamic(0);
	//#pragma omp parallel for num_threads(25) schedule(static)
	#pragma acc kernels loop
	for(int i = 0; i < threadCount; ++i){
		totalDistance = 0;
		for(int j = 0; j < decaps_num/4; ++j){
			totalDistance += tracking[i][j].distance;
		}
		
		execute_permutations_concurrent(decapIndex, totalDistance, &pinmap[i], tracking[i]);
	}

	clock_t time_end = clock();

	cout << "It took " << time_end-time_start << " clicks (" << ((float)(time_end-time_start)/CLOCKS_PER_SEC) << " seconds) for meat of parallel.\n";
	//cout << "Number of Parallel Threads:" << numThreads << " Size of PinMap " << sizeof(pinMap) << ' ' << sizeof(PinMap) << '\n';

}