#pragma once


#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Structs.h"

#include <stdbool.h>

//typedef struct GameObject;

struct STransform {
	Point position;
	Vector2 scale;

	//GameObject* gameObject;
};

typedef struct STransform Transform;

typedef struct Animation {
	char state_name[32];
	IntList sprites;

	int current_index;
	int wait_frame, current_frame;
}Animation;

struct SAnimationList {

	int Count;
	Animation** List;

};

typedef struct SAnimationList AnimationList;




struct SGameObject {

	int id;

	Transform* transform;
	Image *image;
	bool sprite_sheet;
	Vector2 clip_size;
	AnimationList animations;

	char current_state[32];

	void* start;
	void* update;

};


typedef struct SGameObject GameObject;


Animation * get_animation_sprites(GameObject* self, char* state);



GameObject* GameObject_New(Point position, Vector2 scale, Image* image, bool is_sprite_sheet, Vector2 clip_size, Animation* animations, int animations_size, void* start, void* update);

void GameObject_Start(GameObject* self);

void GameObject_Update(GameObject* self);


void GameObject_Draw(GameObject* self);



struct SGameObjectList {

	int Count;
	GameObject** List;

};

typedef struct SGameObjectList GameObjectList;


void initialize_game_object_list(GameObjectList * list);

void add_game_object_to_list(GameObjectList * list, GameObject* game_object);

void delete_game_object_at(GameObjectList * list, int index);

void delete_game_object_from_list(GameObjectList * list, GameObject* game_object);


///

void initialize_animation_list(AnimationList * list);

void add_animation_to_list(AnimationList * list, Animation* game_object);



#endif // !GAMEOBJECT_H
