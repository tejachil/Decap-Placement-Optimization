#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Decap.h"
#include "PinMap.h"

using namespace std;

const string delimiter = " ";

int main(int argc, char **argv){
	unsigned short rows, columns, numberPins, decapDepth, adjacencyCount;
	
	PinMap * pinMap;
	Pin * powerPins;
	Decap * decaps;
	
	string line;
	ifstream pinMapFile ("DSP56311.txt");
	if (pinMapFile.is_open()){
		pinMapFile >> rows >> columns >> numberPins >> decapDepth >> adjacencyCount;
		
		// Create new pinmap with set number of rows and columns
		pinMap = new PinMap(rows, columns, decapDepth);
		powerPins = new Pin[numberPins];
		decaps = new Decap[numberPins];
		
		cout << rows << "\tx\t" << columns << '\t' << numberPins << '\n';
		cout << "Pinmap:\n";
		getline (pinMapFile,line);
		
		short pinCount = 0;
		short rowCount = 0;
		while (getline (pinMapFile,line)){
			if(line.empty())	break;
			
			char tempChar;

			istringstream lineStream(line);
			
			short columnCount = 0;
			while(lineStream >> tempChar){
				if(tempChar >= 48 && tempChar <= 90){
					cout << tempChar;
					powerPins[pinCount].name = tempChar;
					powerPins[pinCount].location.x = columnCount;
					powerPins[pinCount].location.y = rowCount;
					decaps[pinCount].configure(decapDepth, adjacencyCount, &powerPins[pinCount]); 
					++columnCount;
					++pinCount;
				}
				if(tempChar == '.')	++columnCount;
			}
			
			cout << '\n';
			/*size_t indexPos = 0;
			while ((indexPos = line.find(delimiter)) != string::npos) {
				cout << line.substr(0, indexPos);
				//std::cout << token << std::endl;
				line.erase(0, pos + delimiter.length());
			}*/
			
			//cout << line << '\n';
			++rowCount;
		}
		pinMapFile.close();
	}
	else cout << "Unable to open file"; 
	
	return 0;
}
