#include "Utils.h"

#include "Structs.h"

#include <Windows.h>

bool file_exists(char * path)
{
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

char* uint_to_binary(Uint32 toberesolved)
{
	binary[32] = '\0';

	for (int i = 0; i < 31; i++) {
		int r = toberesolved % 2;
		binary[32 - i - 1] = r + /*where numbers start*/ +48;
		if ((toberesolved /= 2) == 1) {
			binary[0] = toberesolved + 48;
			break;
		}
	}

	return &binary[0];
}

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

Vector2 vec2_multiply(Vector2 vec1, float multiplier)
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



//void destroy_after_backside(destroy_info* info)
//{
//	Sleep(info->after_time * 1000);
//
//	while (wait_to_process_on_to_be_destroyed) Sleep(0);
//
//	add_game_object_to_list(&ToBeDestroyed, info->object);
//
//	free(info);
//}


void destroy_after(GameObject* object, float after_time)
{
	DestroyInfo* info = (DestroyInfo*)malloc(sizeof(DestroyInfo));
	info->object = object;
	info->time = after_time;
	info->destroyed = false;

	//while (wait_to_process_on_to_be_destroyed);

	add_member_to_list(&ToBeDestroyed, info);

	//object->destroy_thread_handle = create_thread(destroy_after_backside, info); // destroy after 3 seconds
}

DestroyInfo* find_in_destroy_info(GameObject* object)
{
	for (int i = 0; i < ToBeDestroyed.Count; i++)
	{
		DestroyInfo** list = ToBeDestroyed.List;

		if (list[i]->object == object)
			return list[i];
	}

	return NULL;
}

bool destroy_object(GameObject* object, bool delete_from_destroy_list)
{
	int index = delete_game_object_from_list(&GameObjects, object);

	if (index >= 0)
	{
		if (delete_from_destroy_list)
		{
			DestroyInfo* destroy_info = find_in_destroy_info(object);
			if (destroy_info != NULL)
			{
				delete_member_from_list(&ToBeDestroyed, destroy_info);

				free(destroy_info);
			}
		}
			

		UnloadTexture(object->image);
		//SDL_DestroyTexture(object->image->texture);

		//free(object->image);

		free(object->transform);

		//for (int i = 0; i < object->animations.Count; i++)
		//{
		//	if (object->animations.List[i]->sprites.List != NULL)
		//		free(object->animations.List[i]->sprites.List);

		//	free(object->animations.List[i]);
		//}

		free(object->animations.List);

		//if (delete_from_destroy_list && object->destroy_thread_handle)
		//{
		//	TerminateThread(object->destroy_thread_handle, 0);
		//}

		free(object);


		return true;
	}
		
	return false;
}

