#pragma once

#include <SDL2/SDL.h>
#include "GameObject.h"
#include "Structs.h"

void process_pixel_data(SDL_Surface* surface);

Uint32 get_pixel_data(SDL_Surface *surface, int x, int y);

void put_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

SDL_Surface* load_image(const char* file);

//TODO
GameObject* object_from_pixel_data();