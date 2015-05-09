#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include "Decap.h"
#include "PinMap.h"
#include "DecapPlacement.h"

using namespace std;

const string delimiter = " ";

int main(int argc, char **argv){
	
	unsigned short rows, columns, numberPins, decapDepth, adjacencyCount;
	
	Pin * powerPins;
	Decap * decaps;
	
	short pinCount = 0;
	short rowCount = 0;
	
	string line;
	ifstream pinMapFile ("DSP56311.txt");
	if (pinMapFile.is_open()){
		// Get information from the first line
		pinMapFile >> rows >> columns >> numberPins >> decapDepth >> adjacencyCount;
		
		powerPins = new Pin[numberPins];
		decaps = new Decap[numberPins];

		cout << rows << "x" << columns << " BGA with " << numberPins << " pins. DecapDepth=" << decapDepth << ", AdjacencyCount=" << adjacencyCount << '\n';
		cout << "Possible Decap Placements:\n";
		
		// Clear the rest of the first line from buffer
		getline (pinMapFile,line);
		
		// Go through each row
		while (getline (pinMapFile,line)){
			if(line.empty())	break;

			istringstream lineStream(line);
			
			// Go through each column
			char tempChar;
			short columnCount = 0;
			while(lineStream >> tempChar){
				// If it is a letter or number
				if(tempChar >= 48 && tempChar <= 90){
					cout << tempChar << ": ";
					powerPins[pinCount].name = tempChar;
					powerPins[pinCount].location.x = columnCount + decapDepth;
					powerPins[pinCount].location.y = rowCount + decapDepth;
					decaps[pinCount].configure(rows, columns, decapDepth, adjacencyCount, &powerPins[pinCount]);
					
					for(int i = 0; i < decaps[pinCount].num_placements; ++i){
						cout << '(' << (int)decaps[pinCount].placements[i].x << ',' << (int)decaps[pinCount].placements[i].y << ") ";
					}
					
					cout << '\n';
					++columnCount;
					++pinCount;
				}
				// If it is a dot
				if(tempChar == '.')	++columnCount;
			}
						
			//cout << line << '\n';
			++rowCount;
		}
		pinMapFile.close();
	}
	else cout << "Unable to open file"; 
	
	clock_t time_start, time_end;

	// Sequential Execution
	cout << "\n---------EXECUTING SEQUENTIAL---------\n";
	PinMap * pinMap_sequential = new PinMap(rows, columns, decapDepth);
	Placement * placementTracking_sequential = new Placement[numberPins];
	
	time_start = clock();
	DecapPlacement optimizer_sequential(numberPins, decaps);
	optimizer_sequential.execute_permutations_concurrent(0, 0.0, pinMap_sequential, placementTracking_sequential);
	time_end = clock();

	cout << "It took " << time_end-time_start << " clicks (" << ((float)(time_end-time_start)/CLOCKS_PER_SEC) << " seconds) for sequential.\n";
	cout << "Number of Permutations = " << optimizer_sequential.counter << '\n';
	optimizer_sequential.print_best_pinmap(rows, columns, decapDepth, powerPins);

	// Parallel Execution
	cout << "\n----------EXECUTING PARALLEL----------\n";
	PinMap * pinMap_parallel = new PinMap(rows, columns, decapDepth);
	
	time_start = clock();
	DecapPlacement optimizer_parallel(numberPins, decaps);
	optimizer_parallel.execute_permutations_parallel(pinMap_parallel);
	time_end = clock();
	cout << "It took " << time_end-time_start << " clicks (" << ((float)(time_end-time_start)/CLOCKS_PER_SEC) << " seconds) for parallel.\n";
	cout << "Number of Permutations = " << optimizer_parallel.counter << '\n';
	optimizer_parallel.print_best_pinmap(rows, columns, decapDepth, powerPins);
	
	return 0;
}
