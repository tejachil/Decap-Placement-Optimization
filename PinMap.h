#ifndef PINMAP_H
#define PINMAP_H

#include <stdint.h>

class PinMap{
public:
	char** map;
	PinMap(uint8_t rows, uint8_t columns, uint8_t decapDepth);
	~PinMap();

};

#endif // PINMAP_H
