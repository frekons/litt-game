#pragma once


#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Structs.h"

//typedef struct GameObject;

struct STransform {
	Point position;
	Vector2 scale;

	//GameObject* gameObject;
};

typedef struct STransform Transform;



struct SGameObject {

	int id;

	Transform* transform;
	Image *image;

	void* start;
	void* update;

};

typedef struct SGameObject GameObject;


GameObject* GameObject_New(Point position, Vector2 scale, Image* image, void* start, void* update);

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


#endif // !GAMEOBJECT_H
