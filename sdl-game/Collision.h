#pragma once


//#ifndef COLLISION_H
//
//#define COLLISION_H 1

#include "Structs.h"



bool IsInteractingPoint(BoxCollider col, Point point);

bool IsInteracting(BoxCollider b1, BoxCollider b2);

GameObjectList GetInteractsOfCollider(BoxCollider collider, Point position);

GameObjectList GetInteracts(GameObject* object);

GameObjectList GetInteractsExceptLayer(GameObject* object, int layer);

GameObjectList GetInteractsOnlyLayer(GameObject* object, int layer_mask);

Vector2 collider_center(BoxCollider col);


GameObject* is_on_platform(GameObject* self, int platform_layermask, float tolerance);
GameObject* is_on_something(GameObject* self, int platform_layermask, float tolerance);


bool is_collides_except(GameObject* self, GameObject* except);


//#endif