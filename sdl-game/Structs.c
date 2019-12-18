#include "Structs.h"


Point create_point(float x, float y)
{
	Point vec2 = { x,y };

	return vec2;
}


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



void initialize_list(PointerList * list)
{
	list->Count = 0;
	list->List = NULL;
}

void add_member_to_list(PointerList * list, void* member)
{
	list->Count++;
	list->List = (void**)realloc(list->List, list->Count * sizeof(void*));

	list->List[list->Count - 1] = member;
}


void delete_member_at(PointerList * list, int index)
{
	if (index < 0 || index >= list->Count) return;

	//void* addr = list->List[index];

	for (int i = index; i < list->Count - 1; i++)
		list->List[i] = list->List[i + 1];

	list->Count--;

	list->List = (void**)realloc(list->List, list->Count * sizeof(void*));

	//free(addr);
}

int delete_member_from_list(PointerList * list, void* member)
{
	int index = -1;
	for (int i = 0; i < list->Count; i++)
		if (list->List[i] == member)
		{
			index = i;
			break;
		}

	delete_member_at(list, index);

	return index;
}

int find_member_in_list(PointerList* list, void* member)
{
	for (int i = 0; i < list->Count; i++)
	{
		if (list->List[i] == member)
			return i;
	}

	return -1;
}

PointerList find_differences_in_lists(PointerList * list1, PointerList * list2)
{
	PointerList differenties;
	initialize_list(&differenties);

	//for (int i = 0; i < list1->Count; i++)
	//{
	//	if (find_member_in_list(list2, list1->List[i]) == -1)
	//		add_member_to_list(&differenties, list1->List[i]);
	//}

	for (int i = 0; i < list2->Count; i++)
	{
		if (find_member_in_list(list1, list2->List[i]) == -1)
			add_member_to_list(&differenties, list2->List[i]);
	}

	return differenties;
}