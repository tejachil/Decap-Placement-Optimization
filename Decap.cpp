#include "Decap.h"

Decap::Decap(){
	
}
	
Decap::~Decap(){
	delete [] placements;
}

void Decap::configure(unsigned short decapDepth, unsigned short adjacencyCount, Pin* associatedPin){
	placements = new Location[4*(decapDepth*adjacencyCount)];
	associated_pin = associatedPin;
}
