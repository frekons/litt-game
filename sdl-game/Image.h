#pragma once

#ifndef IMAGE_H
#define IMAGE_H

#include "Structs.h"

#include <SDL2/SDL.h>

Image* LoadTexture(char* path);

void DrawImage(Image* image, Rect rect);

#endif // !IMAGE_H
