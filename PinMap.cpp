#include "PinMap.h"
#include <iostream>
#include <cstring>

using namespace std;

PinMap::PinMap(uint8_t rows, uint8_t columns, uint8_t decapDepth){
	total_rows = rows+2*decapDepth;
	total_columns = columns+2*decapDepth;

	map = new char*[total_rows];
	for(int i = 0; i < total_rows; i++){
		map[i] = new char[total_columns];
		for(int j = 0; j < total_columns; ++j)
			map[i][j] = ' ';
	}
	
}

PinMap::PinMap(){

}

void PinMap::copy(PinMap* pinMap){
	total_rows = pinMap->total_rows;
	total_columns = pinMap->total_columns;
	
	map = new char*[total_rows];
	for(int i = 0; i < total_rows; i++){
		map[i] = new char[total_columns];
		for(int j = 0; j < total_columns; ++j)
			map[i][j] = pinMap->map[i][j];
	}
}

PinMap::~PinMap(){
	for(int i = 0; i < sizeof(map); i++)
		delete[] map[i];
	delete[] map;
}

