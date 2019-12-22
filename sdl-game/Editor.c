#include "Editor.h"

#include <SDL2/SDL_image.h>
#include <string.h>

#include "Game.h"

#include "Draws.h"

//To hold the temporary pixel info for a GameObject
//DEFAULT: WHITE
Uint32 temp = 0xFFFFFFFF;

//Location of the map
const char* mapLocation = "resources/environment/test.png";

GameObjectList InitializeEditor()
{
	map_init();
	process_pixels();
	return list;
}

void RenderEditor() {
	//TODO: DrawButtonWithImageOnScreen için parametreleri olan bir fonksiyonu alan varyantýný yap.
	//DrawButtonWithImageOnScreen()
	//TODO
}

void render_map() {
	for (int y = 0; y < map->h; y++) {
		for (int x = 0; x < map->w; x++) {
			//TODO: DrawButtonOnScreen için parametreleri olan bir fonksiyonu alan varyantýný yap.
			int parameters[3] = { x,y,temp };
			DrawButtonOnScreen("", (Rect) { x * 5, y * 5, 5, 5 }, (Color) {255,255,255,255}, (Color) { 0, 0, 0, 0 }, Font_Minecraft, put_pixel, parameters);
		}
	}
}


void map_init()
{
	map = IMG_Load(mapLocation);
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
void onclick(Uint8 object) {
	switch (object) {
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

	int bpp = map->format->BytesPerPixel;

	for (int y = 0; y < map->h; y++) {
		for (int x = 0; x < map->w; x++) {
			
		}
	}
}

//Uint32 get_pixel_data(SDL_Surface* surface, int x, int y)
//{
//	int bpp = surface->format->BytesPerPixel;
//	/* Here p is the address to the pixel we want to retrieve */
//	Uint8* p = (Uint8*)map->pixels + y * map->pitch + x * bpp;
//
//	switch (bpp) {
//	case 1:
//		return *p;
//		break;
//
//	case 2:
//		return *(Uint16*)p;
//		break;
//
//	case 3:
//		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
//			return p[0] << 16 | p[1] << 8 | p[2];
//		else
//			return p[0] | p[1] << 8 | p[2] << 16;
//		break;
//
//	case 4:
//		return *(Uint32*)p;
//		break;
//
//	default:
//		return 0;       /* shouldn't happen, but avoids warnings */
//	}
//}

void put_pixel(int* parameters) // int x, int y, Uint32 pixel
{
	int x = (int)parameters[0];
	int y = (int)parameters[1];
	Uint32 pixel = (Uint32)parameters[2];

	SDL_LockSurface(map);
	int bpp = map->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)map->pixels + y * map->pitch + x * bpp;

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
	SDL_UnlockSurface(map);
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

void get_rgba(Uint32 pixel_data, Uint8* rgba)
{
	SDL_GetRGBA(pixel_data, map->format, rgba[0], rgba[1], rgba[2], rgba[3]);
}
