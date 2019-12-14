#include "Utils.h"

#include "Structs.h"

#include <Windows.h>


float magnitude(Point point)
{
	return sqrtf(point.x * point.x + point.y * point.y);
}

Point point_lerp(Point point1, Point point2, float time) // time is between 0, 1
{
	Point diff = { point2.x - point1.x, point2.y - point1.y };

	float magn = magnitude(diff);

	if (magn <= 0.1f)
		return point2;

	Point to_return;

	to_return.x = point1.x * (1 - time) + point2.x * time;;
	to_return.y = point1.y * (1 - time) + point2.y * time;;

	return to_return;
}

void* create_thread(void* func, void* parameter) {

	return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, parameter, 0, 0);
}

Vector2 vec2_sum(Point vec1, Vector2 vec2)
{
	return (Vector2) { vec1.x + vec2.x, vec1.y + vec2.y };
}

Vector2 vec2_minus(Vector2 vec1, Vector2 vec2)
{
	return (Vector2) { vec1.x - vec2.x, vec1.y - vec2.y };
}

float clamp(float value, float min, float max)
{
	if (value > max)
		return max;

	if (value < min)
		return min;

	return value;
}

float sign(float value)
{
	if (value > 0)
		return 1;

	if (value < 0)
		return -1;

	return value;
}

bool float_compare(float one, float two, float tolerance)
{
	if (one > two - tolerance && one < two + tolerance)
		return true;

	return false;
}


#include "Game.h"

typedef struct sdestroy_info
{
	GameObject* object;
	float after_time;
}destroy_info;

void destroy_after_background(destroy_info* info)
{
	Sleep(info->after_time * 1000);

	delete_game_object_from_list(&GameObjects, info->object);

	free(info);
}


void destroy_after(GameObject* object, float after_time)
{
	destroy_info* info = (destroy_info*)malloc(sizeof(destroy_info));
	info->object = object;
	info->after_time = after_time;

	create_thread(destroy_after_background, info); // destroy after 3 seconds
}
