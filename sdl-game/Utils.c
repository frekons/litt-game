#include "Structs.h"

Point vec2_lerp(Point point1, Point point2, float time) // time is between 0, 1
{
	Point to_return;

	to_return.x = point1.x * (1 - time) + point2.x * time;;
	to_return.y = point1.y * (1 - time) + point2.y * time;;

	return to_return;
}