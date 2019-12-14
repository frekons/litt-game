#include "Time.h"

#include <SDL2/SDL.h>

float get_time()
{
	return SDL_GetTicks() / 1000.0f;
}