#pragma once

#include <SDL2/SDL.h>

#include "GameObject.h"

#include "Structs.h"

#include "Image.h"

Point process_pixel_data(SDL_Surface* surface, char* object);

GameObject* CreateObject(Vector2 scale, BoxCollider collider, int layer, Image* image, Animation* animations, int animations_size, void* start, void* update);

Uint32 get_pixel_data(SDL_Surface *surface, int x, int y);

void put_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

SDL_Surface* load_image(const char* file);