#include "CrossRoadConfig.h"

CrossRoadConfig::CrossRoadConfig(
	uint8_t north_byte1, uint8_t north_byte2,
	uint8_t east_byte1, uint8_t east_byte2,
	uint8_t south_byte1, uint8_t south_byte2,
	uint8_t west_byte1, uint8_t west_byte2)
	: north(north_byte1, north_byte2),
	east(east_byte1, east_byte2),
	south(south_byte1, south_byte2),
	west(west_byte1, west_byte2) {}