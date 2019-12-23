#include "Common.h"


int volume = 2;

GAME_STATES game_state = INGAME;


void GetDesktopResolution(int* horizontal, int* vertical) // https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
{
	RECT desktop;

	const HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &desktop);

	*horizontal = desktop.right;
	*vertical = desktop.bottom;
}


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

	int screen_width = 1280, screen_height = 720;

	int windows_resolution_width, windows_resolution_height;
	GetDesktopResolution(&windows_resolution_width, &windows_resolution_height);

	set_window_rect(windows_resolution_width / 2 - screen_width / 2, windows_resolution_height / 2 - screen_height / 2, screen_width, screen_height);

	game_window = SDL_CreateWindow("SDL-Game", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);

	camera = create_camera(screen_width, screen_height);

	initialize_list(&ToBeDestroyedTextures);
}