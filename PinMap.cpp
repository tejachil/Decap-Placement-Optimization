#include "PinMap.h"

PinMap::PinMap(uint8_t rows, uint8_t columns, uint8_t decapDepth){
	map = new char*[rows+2*decapDepth];
	for(int i = 0; i < rows+2*decapDepth; i++){
		map[i] = new char[columns+2*decapDepth];
		for(int j = 0; j < columns+2*decapDepth; ++j)
			map[i][j] = ' ';
	}
	
}

PinMap::~PinMap(){
	for(int i = 0; i < sizeof(map); i++)
		delete[] map[i];
	delete[] map;
}

