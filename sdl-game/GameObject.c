#include "GameObject.h"

#include "Game.h"



GameObject* GameObject_New(Point position, Vector2 scale, Image* image, bool is_sprite_sheet, Vector2 clip_size, Animation *animations, int animations_size, void* start, void* update)
{
	GameObject* self = (GameObject*)malloc(sizeof(GameObject));

	self->id = GameObjects.Count;

	Transform *transform = (Transform*)malloc(sizeof(Transform));
	//transform->gameObject = self;
	transform->position = position;
	transform->scale = scale;

	self->transform = transform;


	self->image = image;
	
	self->sprite_sheet = is_sprite_sheet;

	self->clip_size = clip_size;

	strcpy(self->current_state, "idle");


	initialize_animation_list(&self->animations);

	for (int i = 0; i < animations_size; i++)
		add_animation_to_list(&self->animations, &animations[i]);

	

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

	if (!self->sprite_sheet)
	{

		DrawImage(self->image, rect);
	}
	else
	{
		Animation* animation = get_animation_sprites(self, self->current_state);

		if (animation == NULL) {

			SDL_Log("ANIMATION IS NULL!");

			return;
		}


		if (animation->current_index >= animation->sprites.Count)
			animation->current_index = 0;

		DrawClipImage(self->image, rect, create_rect(0,0, self->clip_size.x, self->clip_size.y), animation->sprites.List[animation->current_index]);

		if (++animation->current_frame >= animation->wait_frame)
		{
			animation->current_frame = 0;
			animation->current_index++;
		}


	}


}



Animation * get_animation_sprites(GameObject* self, char* state)
{
	for (int i = 0; i < self->animations.Count; i++)
	{
		if (strcmp(self->animations.List[i]->state_name, state) == 0)
			return self->animations.List[i];
	}

	return NULL;
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

//


void initialize_animation_list(AnimationList * list)
{
	list->Count = 0;
	list->List = NULL;
}

void add_animation_to_list(AnimationList * list, Animation* game_object)
{
	list->Count++;
	list->List = (Animation**)realloc(list->List, list->Count * sizeof(Animation*));

	list->List[list->Count - 1] = game_object;
}