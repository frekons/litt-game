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


//#endif