#include "Structs.h"


float magnitude(Point point)
{
	return sqrtf(point.x * point.x + point.y * point.y);
}

Point vec2_lerp(Point point1, Point point2, float time) // time is between 0, 1
{
	Point to_return;

	Point diff = { point2.x - point1.x, point2.y - point1.y };

	float magn = magnitude(diff);

	if (magn <= 0.001f)
		return point2;

	to_return.x = point1.x * (1 - time) + point2.x * time;;
	to_return.y = point1.y * (1 - time) + point2.y * time;;


	return to_return;
}