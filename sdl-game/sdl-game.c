
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

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "SDL2_ttf.lib")


#undef main


int main(int argc, char* argv[])
{
	set_up_things();

	set_background_color(create_color(0, 0, 255, 255));

	Start(); // don't use Game::GameObjects before this!

	InitializeSounds();
	InitializeDraws();

	deltaTime = 0.016;

	SDL_Event event;
	while (true)
	{

		//__int64 time1, time2;
		//QueryPerformanceCounter((LARGE_INTEGER*)&time1);

		DWORD start_time = timeGetTime();
	
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

		int count = texture_list_to_be_destroyed.Count;

		for (int i = 0; i < count; i++)
		{
			SDL_DestroyTexture(texture_list_to_be_destroyed.List[0]);
			delete_member_at(&texture_list_to_be_destroyed, 0);
		}

		//QueryPerformanceCounter((LARGE_INTEGER*)&time2);

		//__int64 nanoseconds = (time2 - time1);

		//deltaTime = nanoseconds / 10000000.0;

		DWORD end_time = timeGetTime();

		DWORD diff = end_time - start_time;

		Uint32 sleep_time = 16 - diff;

		if (sleep_time > 0 && sleep_time < 1000)
		{
			SDL_Delay(sleep_time);

			end_time = timeGetTime();

			deltaTime = (end_time - start_time) / 1000.0f;

			//QueryPerformanceCounter((LARGE_INTEGER*)&time2);

			//nanoseconds = (time2 - time1);

			//deltaTime = nanoseconds / 10000000.0;
		}
			

		//SDL_Log("fps: %f\n", fps);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_window);
	SDL_Quit();

	return 0;
}