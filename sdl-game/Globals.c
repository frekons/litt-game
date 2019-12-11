#include "Globals.h"

#include <time.h>

void set_window_rect(int x, int y, int w, int h) {

	window_rect.x = x;
	window_rect.y = y;
	window_rect.w = w;
	window_rect.h = h;

}


void set_background_color(Color color) {
	background_color = color;
}


void set_render_color(Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

Color get_render_color()
{
	Color color;
	SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);

	return color;
}



void set_up_things()
{
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);

	set_window_rect(50, 50, 800, 600);

	game_window = SDL_CreateWindow("SDL-Game", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}