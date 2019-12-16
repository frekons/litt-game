#pragma once

//#ifndef GAME_H
//#define GAME_H

#include "GameObject.h"

GameObjectList GameObjects, ToBeDestroyed;

bool wait_to_process_on_to_be_destroyed;

void Start();

void Render();

void create_enemy(int index, Vector2 position);

//#endif // !GAME_H
