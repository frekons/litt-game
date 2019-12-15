#include "Utils.h"

#include "Structs.h"

#include <Windows.h>


float point_magnitude(Point point)
{
	return sqrtf(point.x * point.x + point.y * point.y);
}

float vec2_magnitude(Vector2 point)
{
	return sqrtf(point.x * point.x + point.y * point.y);
}

Point point_lerp(Point point1, Point point2, float time) // time is between 0, 1
{
	Point diff = { point2.x - point1.x, point2.y - point1.y };

	float magn = point_magnitude(diff);

	if (magn <= 0.1f)
		return point2;

	Point to_return;

	to_return.x = point1.x * (1 - time) + point2.x * time;;
	to_return.y = point1.y * (1 - time) + point2.y * time;;

	return to_return;
}

Vector2 vec2_lerp(Vector2 point1, Vector2 point2, float time) // time is between 0, 1
{
	Vector2 diff = { point2.x - point1.x, point2.y - point1.y };

	float magn = point_magnitude((Point) { diff.x, diff.y });

	if (magn <= 0.1f)
		return point2;

	Vector2 to_return;

	to_return.x = point1.x * (1 - time) + point2.x * time;
	to_return.y = point1.y * (1 - time) + point2.y * time;

	return to_return;
}


Vector2 vec2_movetowards(Vector2 point1, Vector2 point2, float speed)
{
	Vector2 a = vec2_minus(point2, point1);
	float magn = vec2_magnitude(a);

	if (magn <= speed || magn == 0.0f)
	{
		return point2;
	}

	Vector2 result;
	result.x = point1.x + a.x / magn * speed;
	result.y = point1.y + a.y / magn * speed;

	return result;
}


float float_lerp(float value1, float value2, float time) // time is between 0, 1
{
	return value1 * (1 - time) + value2 * time;
}

void* create_thread(void* func, void* parameter) {

	return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, parameter, 0, 0);
}

Point point_sum(Point vec1, Vector2 vec2)
{
	return (Point) { vec1.x + vec2.x, vec1.y + vec2.y };
}

Vector2 vec2_multiplier(Vector2 vec1, float multiplier)
{
	return (Vector2) { vec1.x * multiplier, vec1.y * multiplier };
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


void destroy_object(GameObject* object)
{
	delete_game_object_from_list(&GameObjects, object);
}

