#ifndef DECAP_H
#define DECAP_H

#include "BGA.h"

class Decap{
public:
	Location* placements;
	Pin* associated_pin;
	
	Decap();
	~Decap();
	
	void configure(unsigned short decapDepth, unsigned short adjacencyCount, Pin* associatedPin);

};

#endif // DECAP_H
