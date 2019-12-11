
#include "Globals.h"

#include "Draws.h"

#include "Game.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#undef main


int main(int argc, char* argv[])
{
	set_up_things();

	set_background_color(create_color(0, 0, 255, 255));

	Start(); // don't use Game::GameObjects before this!



	SDL_Event event;
	while (true)
	{
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				break;
		}

		// clear screen
		SDL_RenderClear(renderer);
		
		// draw processes

		Render();

		// show
		set_render_color(background_color);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_window);
	SDL_Quit();

	return 0;
}