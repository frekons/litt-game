#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>

struct SIntList {

	int Count;
	int* List;

};

typedef struct SIntList IntList;

void initialize_int_list(IntList * list);

void add_int_to_list(IntList * list, int member);


typedef struct Vector2 {
	float x, y;
}Vector2;

Vector2 create_vec2(float x, float y);

struct SSize {
	int w, h;
};

typedef struct SSize Size;

typedef struct SDL_Rect Rect;

typedef struct SDL_FPoint Point;


Rect create_rect(int x, int y, int w, int h);




struct SColor {
	Uint8 r, g, b, a;
};

typedef struct SColor Color;

Color create_color(int r, int g, int b, int a);



struct SImage {
	SDL_Texture* texture;
	Rect rect;
};

typedef struct SImage Image;



#endif