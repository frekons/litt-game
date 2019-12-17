#pragma once

//#ifndef GAME_H
//#define GAME_H

#include "GameObject.h"

GameObjectList GameObjects;

PointerList ToBeDestroyed;

bool wait_to_process_on_to_be_destroyed;

void Start();

void Render();


GameObject* create_particle(char* image_directory, Vector2 clip_size, Vector2 scale, Point position, int sprite_count, int wait_frame);

void create_enemy(int index, Vector2 position);

//#endif // !GAME_H
