#ifndef BGA_H
#define BGA_H

#include <stdint.h>

struct Location{
	uint8_t x;
	uint8_t y;
};

struct Pin{
	Location location;
	char name;
};

#endif // BGA_H