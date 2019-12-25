#pragma once

//#ifndef GLOBALS_H
//#define GLOBALS_H

#include <SDL2/SDL.h>

#include "Structs.h"


SDL_Window* game_window;

SDL_Renderer* renderer;

Rect window_rect;

Color background_color;

void set_window_rect(int x, int y, int w, int h);

void set_background_color(Color color);

Color get_render_color();

void set_render_color(Color color);

void set_up_things();


float deltaTime;

bool renderable_state;

int last_input;

int volume;

typedef enum {

	MENU = 0,
	EDITOR,
	INGAME,
	PAUSE,
	HOWTOPLAY,
	QUIT

}GAME_STATES;


GAME_STATES game_state;

PointerList ToBeDestroyedTextures;


//#endif // !GLOBALS_H
