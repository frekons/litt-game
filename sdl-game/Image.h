#pragma once

#ifndef IMAGE_H
#define IMAGE_H

#include "Structs.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

Image* LoadTexture(char* path);

//SDL_Texture** SplitTexture(Image* texture, Vector2 split_size, int *size);

void DrawImage(Image* image, Rect rect, bool);
void DrawClipImage(Image* image, Rect rect, Rect clipRect, int index, bool);

#endif // !IMAGE_H
