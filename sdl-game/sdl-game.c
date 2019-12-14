
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

#pragma comment(lib, "ws2_32.lib")


#undef main


int main(int argc, char* argv[])
{
	set_up_things();

	set_background_color(create_color(0, 0, 255, 255));

	Start(); // don't use Game::GameObjects before this!

	MusicInitialize();

	deltaTime = 0.016;

	SDL_Event event;
	while (true)
	{

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

		int sleep_time = (int)((0.016 - deltaTime) * 1000);

		printf("sleep_time: %d\n", sleep_time);

		if (sleep_time > 0)
		{
			printf("sleep!\n");
			SDL_Delay((Uint32)sleep_time);

			QueryPerformanceCounter((LARGE_INTEGER*)&time2);

			nanoseconds = (time2 - time1);

			deltaTime = nanoseconds / 10000000.0;
		}
			

		//SDL_Log("fps: %f\n", fps);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_window);
	SDL_Quit();

	return 0;
}