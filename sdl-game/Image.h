#pragma once

#ifndef IMAGE_H
#define IMAGE_H

#include "Structs.h"

#include <SDL2/SDL.h>

Image* LoadTexture(char* path);

Image** SplitTexture(Image* texture, Vector2 split_size);

void DrawImage(Image* image, Rect rect);

#endif // !IMAGE_H
