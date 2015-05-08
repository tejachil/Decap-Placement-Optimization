#ifndef PINMAP_H
#define PINMAP_H

#include <stdint.h>

class PinMap{
public:
	uint8_t total_rows, total_columns; 

	char** map;
	PinMap(uint8_t rows, uint8_t columns, uint8_t decapDepth);
	PinMap();

	void copy(PinMap* pinMap);
	~PinMap();

};

#endif // PINMAP_H
