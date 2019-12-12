#include "Structs.h"

Vector2 create_vec2(float x, float y)
{
	Vector2 vec2 = { x,y };

	return vec2;
}


Rect create_rect(int x, int y, int w, int h)
{
	Rect rect = { x,y,w,h };

	return rect;
}


Color create_color(int r, int g, int b, int a) {

	Color color = { r,g,b,a };

	return color;
}


void initialize_int_list(IntList * list)
{
	list->Count = 0;
	list->List = NULL;
}

void add_int_to_list(IntList * list, int game_object)
{
	list->Count++;
	list->List = (int*)realloc(list->List, list->Count * sizeof(int));

	list->List[list->Count - 1] = game_object;
}