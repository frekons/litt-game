
#include "Globals.h"

#include "Draws.h"

#include "Game.h"

#include "Audio.h"

#include "Menu.h"

#include "Utils.h"

#include "Editor.h"

#include "Pause.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

#include <Windows.h>

#include <stdio.h>

#include <stdlib.h>

#include <time.h>

// Winmm.lib;SDL2.lib;legacy_stdio_definitions.lib;SDL2_image.lib;SDL2_mixer.lib
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_ttf.lib")


#undef main

void _back_to_mainmenu()
{
	game_state = MENU;
}

int main(int argc, char* argv[])
{
	game_state = MENU;

	set_up_things();

	set_background_color(create_color(0, 0, 255, 255));

	//Start(); // don't use Game::GameObjects before this!

	InitializeSounds();
	InitializeDraws();
	InitializeImage();
	//InitializeEditor();

	deltaTime = 0.016;

	SDL_Event event;
	while (true)
	{

		//__int64 time1, time2;
		//QueryPerformanceCounter((LARGE_INTEGER*)&time1);

		DWORD start_time = timeGetTime();
	
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				game_state = QUIT;
			}

			if (event.type == SDL_MOUSEMOTION)
			{
				Uint32 val = SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

			}

			if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				Uint32 val = SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

				mouse_button_mask = SDL_BUTTON(val);
			}

			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_LEFT:
					last_input = SDL_SCANCODE_LEFT;

					break;

				case SDL_SCANCODE_RIGHT:
					last_input = SDL_SCANCODE_RIGHT;

					break;

					default:
						break;
				}
			}

			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_LEFT:
					last_input = SDL_SCANCODE_RIGHT;

					break;

				case SDL_SCANCODE_RIGHT:
					last_input = SDL_SCANCODE_LEFT;

					break;

				default:
					break;
				}
			}

		}

		if (game_state == QUIT)
			break;

		// clear screen
		SDL_RenderClear(renderer);

		for (int i = 0; i < ToBeDestroyedTextures.Count; i++)
		{
			SDL_DestroyTexture(ToBeDestroyedTextures.List[0]);
			delete_member_at(&ToBeDestroyedTextures, 0);
		}
		
		// draw processes
		renderable_state = true;

		if (game_state == MENU)
		{
			RenderMenu();
		}
		else if(game_state == INGAME)
		{
			Render();
		}
		else if (game_state == EDITOR) {
			RenderEditor();
		}
		else if (game_state == PAUSE)
		{
			RenderPauseMenu();
		}
		else if (game_state == HOWTOPLAY)
		{
			DrawFilledRectangleOnScreen((Rect) { camera->width / 2 - 300, camera->height / 2 - 200, 600,400 }, (Color) { 0, 0, 0, 255 });

			int index = 2;
			DrawTextOnScreen("Saga dogru hareket:  Sag ok tusu", (Vector2) { camera->width / 2, camera->height / 2 - 200 + 50 * index++}, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
			DrawTextOnScreen("Sola dogru hareket:  Sol ok tusu", (Vector2) { camera->width / 2, camera->height / 2 - 200 + 50 * index++}, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
			DrawTextOnScreen("Ziplama:  Yukari ok tusu (iki kere ziplanabilir)", (Vector2) { camera->width / 2, camera->height / 2 - 200 + 50 * index++}, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
			DrawTextOnScreen("Ates etme:  X tusu", (Vector2) { camera->width / 2, camera->height / 2 - 200 + 50 * index++}, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
			DrawTextOnScreen("Kayma:  Z tusu", (Vector2) { camera->width / 2, camera->height / 2 - 200 + 50 * index++}, (Color) { 255, 255, 255, 255 }, Font_Minecraft);


			DrawButtonOnScreen("Anladim!", (Rect) { camera->width / 2 - 300, camera->height / 2 + 200, 600, 100 }, (Color) { 50, 50, 50, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, _back_to_mainmenu, NULL);

		}

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

		Uint32 sleep_time = 15 - diff; // 15ms = 1000 / 15 = 66,6 fps

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