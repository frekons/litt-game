#pragma once

#include "Structs.h"

#include "SDL2/SDL_image.h"

// HEX
Uint32 BLACK;
Uint32 WHITE;

// Additional
Color Black;
Color White;
Color Red;
Color Invisible;

// Characters
Color Player;

// Enemies
Color Bat;
Color Archer;
Color Skeleton;
Color Boss;

// Terrain
Color Dirt;
Color Grass;

Color Stone1;
Color Stone2;
Color Stone3;
Color Stone4;
Color Stone5;

void init_colors();

bool compare_colors(Color color1, Color color2);