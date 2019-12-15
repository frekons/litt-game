#include "Editor.h"

#include <SDL2/SDL_image.h>
#include <string.h>

#include "Game.h"

SDL_Surface* load_image(const char* file)
{
	SDL_Surface *surf = IMG_Load(file);

	return surf;
}

Point process_pixel_data(SDL_Surface* surface, char* object) {
	Uint8 red, green, blue, alpha;

	int bpp = surface->format->BytesPerPixel;

	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			SDL_GetRGBA(get_pixel_data(surface, x, y), surface->format, &red, &green, &blue, &alpha);

			if (red == 0 && green == 38 && blue == 255 && !memcmp(object, "player", 6)) {
				return (Point) { x * 10, y * 10 };
			}
		}
	}
}

GameObject* CreateObject(Vector2 scale, BoxCollider collider, int layer, Image* image, Animation* animations, int animations_size, void* start, void* update)
{
	SDL_Surface* surface = load_image("resources/environment/test.png");

	return GameObject_New(GameObjects.Count, process_pixel_data(surface, "player"), scale, collider, layer, image, animations, animations_size, start, update);
}

Uint32 get_pixel_data(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

void put_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;
	}
}


