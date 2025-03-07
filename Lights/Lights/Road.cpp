#include "Road.h"

Road::Road(uint8_t byte1, uint8_t byte2){
	/*
	O = Orientation
	S = Straight
	L = Left
	R = Right
	Gt = Green time
	P = Pedestrians
	
	byte1:
	| O | O | S | L | R | Gt | Gt | Gt |
	
	byte2:
	| Gt | Gt | Gt | Gt | Gt | Gt | Gt | P | 
	*/
	straight = (byte1 & (1 << 5));
	left = (byte1 & (1 << 4));
	right = (byte1 & (1 << 3));
	green_light = ((byte1 & 0x07) << 7) | ((byte2 & 0xFE) >> 1);
	pedestrians = (byte2 & (1 << 0));
}