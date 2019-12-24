#include "Editor.h"

#include <SDL2/SDL_image.h>
#include <string.h>

#include "Game.h"

#include "Draws.h"

#include "Colors.h"

#include "json/JSON_checker.h"

#define SavePNG(surface, file) \
        SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "rb+"), 1)

//To hold the temporary pixel info for a GameObject
//DEFAULT: WHITE
Uint32 temp = 0xFFFFFFFF;

//Location of the map
const char* mapLocation = "resources/environment/test.png";

int scale = 15;

_Bool init = 0;

int once = 0;

void InitializeEditor()
{
	if (!init) {
		init++;
		map_init();
	}
	camera->position.x += 30;
}

void RenderEditor() {
	render_map();

	int parameters[] = { 0 };
	DrawButtonWithImageOnScreen(NULL, "resources/players/player.png", (Rect) { 0, 500, 80, 100 }, Black, White, NULL, onclick, parameters);
	
	parameters[0] = 1;
	DrawButtonWithImageOnScreen(NULL, "resources/enemies/enemytwo.png", (Rect) { 100, 500, 80, 100 }, Black, White, NULL, onclick, parameters);
}

GameObjectList SaveEditor() {
	process_pixels();
}

PointerList GetMap(char* png_path)
{
	PointerList list;
	initialize_list(&list);

	
}


void render_map() {
	if (!init) {
		init++;
		map_init();
	}
	for (int y = 0; y < maps->h; y++) {
		for (int x = 0; x < maps->w; x++) {

			int parameters[3] = { x,y, temp};

			if (!once++) printf("%X", get_pixel_data(7, 6));

			DrawInteractiveRectangleOnScreen((Rect) {x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale}, to_color(get_pixel_data(x,y)), put_pixel, parameters);
			
			//DrawRectangleOnScreen((Rect){ x * scale, y * scale, scale, scale }, White);
		}
	}
	SDL_FreeSurface(maps);
	init--;
}


void map_init()
{
	maps = IMG_Load(mapLocation);
}

/**
Obje tipleri:
	0 - player
	1 - yarasa
	2 - iskelet
	3 - þövalye
	4 - boss
	5 - trap1
	6 - trap2
	7 - dirt
	8 - grass
*/
void onclick(int* object) {
	switch (object[0]) {
	case 0:
		temp = 0xFF1900FF;
		break;
	case 1:
		temp = 0xFFFF0010;
		break;
	case 2:
		temp = 0xFFC1000C;
		break;
	case 3:
		temp = 0xFF820008;
		break;
	case 4:
		temp = 0xFF000000;
		break;
	case 5:
		temp = 0xFF9E5C38;
		break;
	case 6:
		temp = 0xFF00821A;
		break;
	case 7:
		temp = 0xFFFF00FA;
		break;
	case 8:
		temp = 0xFFA300A0;
		break;
	}
}
void process_pixels() {
	Uint8 rgba[4]; //0 - r, 1 - g, 2 - b, 3 - a

	int bpp = maps->format->BytesPerPixel;

	for (int y = 0; y < maps->h; y++) {
		for (int x = 0; x < maps->w; x++) {
			
		}
	}
}

Uint32 get_pixel_data(int x, int y)
{
	Lock(maps);
	int bpp = maps->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)maps->pixels + y * maps->pitch + x * bpp;

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
	Unlock(maps);
}

void put_pixel(int* parameters) // int x, int y, Uint32 pixel
{
	int x = (int)parameters[0];
	int y = (int)parameters[1];
	Uint32 pixel = (Uint32)parameters[2];

	Lock(maps);
	int bpp = maps->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)maps->pixels + y * maps->pitch + x * bpp;

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

	Unlock(maps);

	//Lock(maps);

	SavePNG(maps, mapLocation);

	//Unlock(maps);
}

Uint32 get_object_color(Uint8 red, Uint8 green, Uint8 blue)
{
	Uint32 data = 0;
	data = (Uint32)255 << 24;
	data += (Uint32)red << 16;
	data += (Uint32)green << 8;
	data += (Uint32)blue << 0;

	return data;
}

Color to_color(Uint32 pixel_data)
{
	Color temp;

	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		temp.b = (pixel_data >> 24) & 0xff;
		temp.g = (pixel_data >> 16) & 0xff;
		temp.r = (pixel_data >> 8) & 0xff;
		temp.a = pixel_data & 0xff;
	}
	else {
		temp.a = (pixel_data >> 24) & 0xff;
		temp.r = (pixel_data >> 16) & 0xff;
		temp.g = (pixel_data >> 8) & 0xff;
		temp.b = pixel_data & 0xff;
	}
	
	return temp;
}

void Unlock(SDL_Surface* surface) {
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}

void Lock(SDL_Surface* surface) {
	if (SDL_MUSTLOCK(surface)) {
		if (SDL_LockSurface(surface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}
}