#ifndef CROSSROADCONFIG_H_
#define CROSSROADCONFIG_H_

#include "Road.h"

// CrossRoadConfig object that stores all 4 roads
class CrossRoadConfig{
public:
	Road north;
	Road east; 
	Road south;
	Road west;
	
	CrossRoadConfig() : north(0, 0), east(0, 0), south(0, 0), west(0, 0) {}
	
	CrossRoadConfig(
		uint8_t north_byte1, uint8_t north_byte2,
		uint8_t east_byte1, uint8_t east_byte2,
		uint8_t south_byte1, uint8_t south_byte2,
		uint8_t west_byte1, uint8_t west_byte2
	);
	
};


#endif