#include "GameObject.h"

#include "Game.h"



GameObject* GameObject_New(Point position, Vector2 scale, Image* image, void* start, void* update)
{
	GameObject* self = (GameObject*)malloc(sizeof(GameObject));

	self->id = GameObjects.Count;

	Transform *transform = (Transform*)malloc(sizeof(Transform));
	//transform->gameObject = self;
	transform->position = position;
	transform->scale = scale;

	self->transform = transform;


	self->image = image;
	

	self->start = start;
	self->update = update;

	add_game_object_to_list(&GameObjects, self);

	GameObject_Start(self);
}

void GameObject_Start(GameObject* self)
{
	if (self->start != NULL)
	{
		typedef void func(GameObject*);
		func* f = (func*)self->start;

		f(self);
	}
}


void GameObject_Update(GameObject* self)
{
	if (self->update != NULL)
	{
		typedef void func(GameObject*);
		func* f = (func*)self->update;

		f(self);
	}
}


void GameObject_Draw(GameObject* self)
{
	if (self == NULL || self->image == NULL) return;

	Rect rect = self->image->rect;
	rect.x = self->transform->position.x;
	rect.y = self->transform->position.y;

	rect.w *= self->transform->scale.x;
	rect.h *= self->transform->scale.y;

	DrawImage(self->image, rect);

}





////////////////// LIST


void initialize_game_object_list(GameObjectList * list)
{
	list->Count = 0;
	list->List = NULL;
}

void add_game_object_to_list(GameObjectList * list, GameObject* game_object)
{
	list->Count++;
	list->List = (GameObject**)realloc(list->List, list->Count * sizeof(GameObject*));

	list->List[list->Count - 1] = game_object;
}

void delete_game_object_at(GameObjectList * list, int index)
{
	if (index < 0 || index >= list->Count) return;

	for (int i = index; i < list->Count - 1; i++)
		list->List[i] = list->List[i + 1];

	list->Count--;

	list->List = (GameObject**)realloc(list->List, list->Count * sizeof(GameObject*));
}

void delete_game_object_from_list(GameObjectList * list, GameObject* game_object)
{
	int index = -1;
	for (int i = 0; i < list->Count; i++)
		if (list->List[i] == game_object)
		{
			index = i;
			break;
		}

	delete_game_object_at(list, index);
}