#include "GameObject.h"

#include "Game.h"

#include "Image.h"

#include "Time.h"

#include "Globals.h"

#include "Collision.h"

#include <Windows.h>

Camera* create_camera(int screen_width, int screen_height)
{
	Camera* cam = (Camera*)malloc(sizeof(Camera));

	cam->position.x = 0;
	cam->position.y = 0;

	cam->width = screen_width;
	cam->height = screen_height;

	cam->shake_force = 0;
	cam->shake_time = 0;

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


	self->velocity = (Vector2) { 0, 0 };
	self->extra_velocity = (Vector2) { 0, 0 };
	self->attack_in_seconds_counter = 0;
	self->dash_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
	self->destroy_thread_handle = 0;

	initialize_list(&self->collider.onEnter);
	initialize_list(&self->collider.onExit);

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


PointerList GameObject_BeforePhysics(GameObject* self)
{
	//if (self->collider.onEnter.Count == 0 && self->collider.onExit.Count == 0)
	//	return (PointerList) { 0, 0 };

	GameObjectList objects = GetInteracts(self);

	return *(PointerList*)&objects;
}


GameObject* GameObject_Update(GameObject* self)
{
	if (self->update != NULL)
	{
		typedef GameObject* func(GameObject*);
		func* f = (func*)self->update;

		return f(self);
	}

	return self;
}

void GameObject_Physics(GameObject* self, PointerList before_interacts)
{
	//if (self->collider.onEnter.Count == 0 && self->collider.onExit.Count == 0)
	//	return;

	GameObjectList objects = GetInteracts(self);

	PointerList interacts = *(PointerList*)&objects;

	PointerList differents = find_differences_in_lists(&before_interacts, &interacts);

	for (int j = 0; j < differents.Count; j++) // onEnter event
	{
		GameObject* obj = (GameObject*)differents.List[j];

		for (int i = 0; i < self->collider.onEnter.Count; i++)
		{
			typedef void func(GameObject*, GameObject*);
			func* f = (func*)self->collider.onEnter.List[i];

			f(self, obj);
		}

		for (int i = 0; i < obj->collider.onEnter.Count; i++)
		{
			typedef void func(GameObject*, GameObject*);
			func* f = (func*)obj->collider.onEnter.List[i];

			f(obj, self);
		}
	}


	differents = find_differences_in_lists(&interacts, &before_interacts);

	for (int j = 0; j < differents.Count; j++) // onEnter event
	{

		GameObject* obj = (GameObject*)differents.List[j];

		for (int i = 0; i < self->collider.onExit.Count; i++)
		{
			typedef void func(GameObject*, GameObject*);
			func* f = (func*)self->collider.onExit.List[i];

			f(self, obj);
		}

		for (int i = 0; i < obj->collider.onExit.Count; i++)
		{
			typedef void func(GameObject*, GameObject*);
			func* f = (func*)obj->collider.onExit.List[i];

			f(obj, self);
		}
	}
}

void shake_camera(float force, float time)
{
	camera->shake_force = force;
	camera->shake_time = time;
}


void GameObject_Draw(GameObject* self)
{
	if (self == NULL || self->image == NULL) return;

	Rect rect;
	rect.x = self->transform->position.x - (camera->position.x - camera->width / 2);
	rect.y = self->transform->position.y - (camera->position.y - camera->height / 2);
	
	if (camera->shake_time > 0 && camera->shake_force != 0)
	{
		float x = (float)rand() / (float)(RAND_MAX / (camera->shake_force * 2.0f)) - camera->shake_force;
		float y = (float)rand() / (float)(RAND_MAX / (camera->shake_force * 2.0f)) - camera->shake_force;

		rect.x -= x;
		rect.y -= y;

		camera->shake_time -= deltaTime;
	}


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

bool wait_to_make_process_on_gameobject_list = false;

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
	wait_to_make_process_on_gameobject_list = true;

	if (index < 0 || index >= list->Count) {
		wait_to_make_process_on_gameobject_list = false;
		return;
	}

	//GameObject* addr = list->List[index];

	for (int i = index; i < list->Count - 1; i++)
		list->List[i] = list->List[i + 1];

	list->Count--;

	list->List = (GameObject**)realloc(list->List, list->Count * sizeof(GameObject*));

	//free(addr);

	wait_to_make_process_on_gameobject_list = false;
}

int delete_game_object_from_list(GameObjectList * list, GameObject* game_object)
{
	while (wait_to_make_process_on_gameobject_list) Sleep(1);

	wait_to_make_process_on_gameobject_list = true;

	int index = -1;
	for (int i = 0; i < list->Count; i++)
		if (list->List[i] == game_object)
		{
			index = i;
			break;
		}

	//printf("index: %d\n", index);

	if (index < 0) {

		wait_to_make_process_on_gameobject_list = false;

		return index;
	}

	delete_game_object_at(list, index);

	return index;
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


bool set_animator_state(GameObject* self, char* state, float time, bool ignore_move, bool dont_check) // time is in seconds
{
	if (!dont_check)
	{
		if (compare_animator_state(self, state))
			return false;
	}


	if (ignore_move) // logical
		self->ignore_movement = true;

	float current_time = get_time();

	if (!dont_check)
		if (self->ignore_movement_time > current_time) return false;

	self->ignore_movement_time = current_time + time;

	Animation* animation = get_animation_sprites(self, state);

	if (animation)
		animation->current_index = 0;

	strcpy(self->current_state, state);

	return true;
}
