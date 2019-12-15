#include "GameObject.h"

#include "Game.h"

#include "Image.h"

#include "Time.h"

#include "Globals.h"

Camera* create_camera(int screen_width, int screen_height)
{
	Camera* cam = (Camera*)malloc(sizeof(Camera));

	cam->position.x = 0;
	cam->position.y = 0;

	cam->width = screen_width;
	cam->height = screen_height;

	return cam;
}


GameObject* GameObject_New(int id, Point position, Vector2 scale, BoxCollider collider, int layer, Image* image, Animation *animations, int animations_size, void* start, void* update)
{
	GameObject* self = (GameObject*)malloc(sizeof(GameObject));

	self->id = id;

	Transform *transform = (Transform*)malloc(sizeof(Transform));
	//transform->gameObject = self;
	transform->position = position;
	transform->scale = scale;
	transform->left = false;

	self->transform = transform;

	self->collider = collider;

	self->image = image;

	self->layer = layer;
	

	strcpy(self->current_state, "idle");


	initialize_animation_list(&self->animations);

	for (int i = 0; i < animations_size; i++)
		add_animation_to_list(&self->animations, &animations[i]);

	

	self->start = start;
	self->update = update;

	add_game_object_to_list(&GameObjects, self);

	GameObject_Start(self);

	return self;
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

	Rect rect;
	rect.x = self->transform->position.x - (camera->position.x - camera->width / 2);
	rect.y = self->transform->position.y - (camera->position.y - camera->height / 2);

	if (!self->image->sprite_sheet)
	{
		rect.w = self->image->rect.w * self->transform->scale.x;
		rect.h = self->image->rect.h *  self->transform->scale.y;

		DrawImage(self->image, rect, self->transform->left);
	}
	else
	{
		rect.w = self->image->clip_size.x *self->transform->scale.x;
		rect.h = self->image->clip_size.y *  self->transform->scale.y;



		Animation* animation = get_animation_sprites(self, self->current_state);

		if (animation == NULL) {

			SDL_Log("No animation found named '%s'!", self->current_state);

			return;
		}


		if (animation->current_index >= animation->sprites.Count)
			animation->current_index = animation->loop ? 0 : animation->sprites.Count - 1;

		DrawClipImage(self->image, rect, create_rect(0,0, self->image->clip_size.x, self->image->clip_size.y), animation->sprites.List[animation->current_index], self->transform->left);

		float fps = 1.0f / deltaTime;

		if ( ++animation->current_frame >= animation->wait_frame * (fps / 60.0f) )
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

	GameObject* addr = list->List[index];

	for (int i = index; i < list->Count - 1; i++)
		list->List[i] = list->List[i + 1];

	list->Count--;

	list->List = (GameObject**)realloc(list->List, list->Count * sizeof(GameObject*));

	free(addr);
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


bool compare_animator_state(GameObject* self, char* state) // time is in seconds
{
	return strcmp(self->current_state, state) == 0;
}


bool set_animator_state(GameObject* self, char* state, float time, bool ignore_move) // time is in seconds
{
	if (compare_animator_state(self, state))
		return false;

	if (ignore_move) // logical
		self->ignore_movement = true;

	float current_time = get_time();

	if (self->ignore_movement_time > current_time) return false;

	self->ignore_movement_time = current_time + time;

	Animation* animation = get_animation_sprites(self, state);

	if (animation)
		animation->current_index = 0;

	strcpy(self->current_state, state);

	return true;
}
