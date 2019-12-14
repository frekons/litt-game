
#include "Globals.h"

#include "Draws.h"

#include "Game.h"

#include "Audio.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#undef main


int main(int argc, char* argv[])
{
	set_up_things();

	set_background_color(create_color(0, 0, 255, 255));

	Start(); // don't use Game::GameObjects before this!

	MusicInitialize();

	SDL_Event event;
	while (true)
	{
		//__int64 freq;
		//QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		//double nanoseconds_per_count = 1.0e9 / *(double*)&freq;

		__int64 time1, time2;
		QueryPerformanceCounter((LARGE_INTEGER*)&time1);
	
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				break;
		}

		// clear screen
		SDL_RenderClear(renderer);
		
		// draw processes
		renderable_state = true;

		Render();

		renderable_state = false;
		// show
		set_render_color(background_color);
		SDL_RenderPresent(renderer);

		QueryPerformanceCounter((LARGE_INTEGER*)&time2);

		__int64 nanoseconds = (time2 - time1);

		deltaTime = nanoseconds / 10000000.0;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_window);
	SDL_Quit();

	return 0;
}