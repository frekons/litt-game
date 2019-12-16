#pragma once

//#ifndef GAME_H
//#define GAME_H

#include "GameObject.h"

GameObjectList GameObjects, ToBeDestroyed;

bool wait_to_process_on_to_be_destroyed;

void Start();

void Render();


//#endif // !GAME_H
