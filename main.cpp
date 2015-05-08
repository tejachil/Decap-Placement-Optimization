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

		cout << rows << "\tx\t" << columns << '\t' << numberPins << '\n';
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
	
	PinMap * pinMap_sequential = new PinMap(rows, columns, decapDepth);
	Placement * placementTracking_sequential = new Placement[numberPins];
	
	clock_t time_start = clock();
	DecapPlacement optimizer(numberPins, decaps);
	//optimizer.execute_permutations_concurrent(0, 0.0, pinMap_sequential, placementTracking_sequential);
	clock_t time_end = clock();

	cout << "It took " << time_end-time_start << " clicks (" << ((float)(time_end-time_start)/CLOCKS_PER_SEC) << " seconds) for sequential.\n";

	cout << "EXECUTING PARALLEL\n";
	PinMap * pinMap_parallel = new PinMap(rows, columns, decapDepth);
	
	time_start = clock();
	optimizer.execute_permutations_parallel(pinMap_parallel);
	time_end = clock();
	cout << "It took " << time_end-time_start << " clicks (" << ((float)(time_end-time_start)/CLOCKS_PER_SEC) << " seconds) for parallel.\n";

	PinMap * pinMap = pinMap_sequential;
	Placement * placementTracking = placementTracking_sequential;

	for(int i = 0; i < rows+2*decapDepth; i++){
		for(int j = 0; j < columns+2*decapDepth; ++j)
			if(i >= decapDepth && i < rows+decapDepth && j >= decapDepth && j < columns+decapDepth)
				pinMap->map[i][j] = '.';
			else	pinMap->map[i][j] = ' ';
	}
	
	for(int i = 0; i < pinCount; ++i){
		pinMap->map[powerPins[i].location.y][powerPins[i].location.x] = powerPins[i].name;
		pinMap->map[decaps[i].placements[placementTracking[i].best_index].y][decaps[i].placements[placementTracking[i].best_index].x] = decaps[i].associated_pin->name;
	}
	
	for(int i = 0; i < rows+2*decapDepth; i++){
		for(int j = 0; j < columns+2*decapDepth; ++j)
			cout << pinMap->map[i][j] << ' ';
		cout << '\n';
	}
	
	return 0;
}
