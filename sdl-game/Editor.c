#include "Editor.h"

#include <SDL2/SDL_image.h>
#include <string.h>

#include "Game.h"

#include "Draws.h"

#include "Colors.h"

#include "json/JSON_checker.h"

#include "Image.h"

#include "Globals.h"

#include <Windows.h>

#define SavePNG(surface, file) \
        SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "wb+"), 1)

//To hold the temporary pixel info for a GameObject
//DEFAULT: WHITE
Uint32 temp = 0xFFFFFFFF;

//Location of the map
char* mapLocation = NULL;

int scale = 15;

_Bool init = 0;

void InitializeEditor(char* location)
{
	mapLocation = location;

	map_init(location);

	init_colors();
	camera->position.x = 600.0;
}

void is_selected(Color object_color, Rect object_rect) {
	if (compare_colors(object_color, to_color(temp))) {
		DrawRectangleOnScreen(object_rect, Red);
	}
	else {
		DrawRectangleOnScreen(object_rect, Black);
	}
}

void draw_selector_box(int* parameters, char* path, Rect object, Vector2 clip, Rect rect, Rect clip_rect, Color property, bool is_sprite) 
{
	DrawInteractiveRectangleOnScreen(object, Invisible, onclick, parameters);
	is_selected(property, object);
	DrawClipImage(LoadTexture(path, is_sprite, clip), rect, clip_rect, 0, false);
	DrawFilledRectangleOnScreen((Rect) { object.x + 32, object.y + scale * 8, scale, scale }, property);
	DrawRectangleOnScreen((Rect) { object.x + 32, object.y + scale * 8, scale, scale }, White);
}

/**
Obje tipleri:
	0 - player
	1 - yarasa
	2 - iskelet
	3 - archer
	4 - boss
	5 - trap1
	6 - trap2
	7 - dirt
	8 - grass
*/
void render_selector() {
	// Player
	int parameters[] = { 0 };
	Rect object = {0,500,80,100};
	draw_selector_box(parameters, "resources/players/player.png", object, (Vector2) { 48, 48 }, (Rect) { 0, 500, 80, 100 }, (Rect) { 0, 0, 48, 48 }, Player, true);

	// Bat
	parameters[0] = 1;
	object.x += 100;
	draw_selector_box(parameters, "resources/enemies/enemytwo.png", object, (Vector2) { 16, 16 }, (Rect) { 100, 500, 80, 100 }, (Rect) { 0, 0, 16, 16 }, Bat, true);

	// Skeleton
	parameters[0] = 2;
	object.x += 100;
	draw_selector_box(parameters, "resources/enemies/enemy.png", object, (Vector2) { 43, 38 }, (Rect) { 200, 500, 80, 100 }, (Rect) { 0, 0, 43, 38 }, Skeleton, true);

	// Archer
	parameters[0] = 3;
	object.x += 100;
	draw_selector_box(parameters, "resources/enemies/enemy3.png", object, (Vector2) { 32, 32 }, (Rect) { 270, 480, 130, 150 }, (Rect) { 0, 0, 32, 32 }, Archer, true);

	//Boss
	parameters[0] = 4;
	object.x += 100;
	draw_selector_box(parameters, "resources/enemies/boss.png", object, (Vector2) { 96, 96 }, (Rect) { 350, 460, 192, 200 }, (Rect) { 0, 0, 96, 96 }, Boss, true);

	// Stone-Ground
	parameters[0] = 9;
	object.x += 100;
	draw_selector_box(parameters, "resources/environment/stone_ground.png", object, (Vector2) { 417, 417 }, (Rect) { 513, 525, 50, 50 }, (Rect) { 0, 0, 417, 417 }, Stone1, false);

	// Stone - Left Corner
	parameters[0] = 10;
	object.x += 100;
	draw_selector_box(parameters, "resources/environment/stone_ground_corner_left.png", object, (Vector2) { 417, 417 }, (Rect) { 613, 525, 50, 50 }, (Rect) { 0, 0, 417, 417 }, Stone2, false);

	// Stone - Right Corner
	parameters[0] = 11;
	object.x += 100;
	draw_selector_box(parameters, "resources/environment/stone_ground_corner_right.png", object, (Vector2) { 417, 417 }, (Rect) { 713, 525, 50, 50 }, (Rect) { 0, 0, 417, 417 }, Stone3, false);

	// Stone - Left
	parameters[0] = 12;
	object.x += 100;
	draw_selector_box(parameters, "resources/environment/stone_ground_left.png", object, (Vector2) { 417, 417 }, (Rect) { 813, 525, 50, 50 }, (Rect) { 0, 0, 417, 417 }, Stone4, false);

	//Stone - Right
	parameters[0] = 13;
	object.x += 100;
	draw_selector_box(parameters, "resources/environment/stone_ground_right.png", object, (Vector2) { 417, 417 }, (Rect) { 913, 525, 50, 50 }, (Rect) { 0, 0, 417, 417 }, Stone5, false);

	// Delete
	parameters[0] = 14;
	object.x = camera->width - 100;
	object.h = 80;
	DrawButtonOnScreen("Sil", object, Black, White, Font_Minecraft, onclick, parameters);
	is_selected(White, object);
}

void navigator_onclick(int* dir) {
	switch (dir[0]) {
	case 0:
		if(camera->position.x <= 600.0f) camera->position.x += scale*32;
		printf("%f\n", camera->position.x);
		break;
	case 1:
		if (camera->position.x >= -6120.0f) camera->position.x -= scale * 32;
		printf("%f\n", camera->position.x);
		break;
	case 2:
		game_state = MENU;
		//SDL_FreeSurface(maps);
	}
}

void render_navigator() {
	int parameters[1] = { 0 };
	//Left
	DrawInteractiveRectangleOnScreen((Rect) { 0, 650, 40, 40 }, (Color) { 0, 0, 0, 0 }, navigator_onclick, parameters);
	DrawImage(LoadTexture("resources/additional/arrow.png", false, (Vector2) { 512, 512 }), (Rect) { 0, 650, 40, 40 }, true);

	parameters[0] = 1;
	//Right
	DrawInteractiveRectangleOnScreen((Rect) { camera->width-60, 650, 40, 40 }, (Color) { 0, 0, 0, 0 }, navigator_onclick, parameters);
	DrawImage(LoadTexture("resources/additional/arrow.png", false, (Vector2) { 512, 512 }), (Rect) { camera->width - 60, 650, 40, 40 }, false);

	//Back
	parameters[0] = 2;
	DrawButtonOnScreen("Geri Don", (Rect) { camera->width - camera->width / 2 - 220, 650, 120, 40 }, Black, White, Font_Minecraft, navigator_onclick, parameters);

	parameters[0] = 2;
	DrawButtonOnScreen("Haritayi Kaydet", (Rect) { camera->width - camera->width / 2, 650, 200, 40 }, Black, White, Font_Minecraft, SaveMap, NULL);
	
}

void RenderEditor() {
	render_navigator();
	render_map();	
	render_selector();
}

void SaveMap()
{
	temp = 0xFFFFFF00;

	int result = SavePNG(maps, mapLocation);

	is_selected(create_color(255, 255, 0, 255), (Rect) { camera->width - camera->width / 2, 650, 200, 40 });

	if (result != 0)
		printf("Error when saving file, Error: %s\n", SDL_GetError());

	Sleep(100);

	temp = 0xFFFFFFFF;
}

void InitializeMap(char* map_location)
{
	init_colors();

	map_init(map_location);

	int scaleFactor = 82;

	for (int y = 0; y < maps->h; y++) {
		for (int x = 0; x < maps->w; x++) {
			if (compare_colors(to_color(get_pixel_data(x, y)), Player)) {
				create_player(create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Skeleton)) {
				create_enemy(0, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Bat)) {
				create_enemy(1, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Archer)) {
				create_enemy(2, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Boss)) {
				create_enemy(3, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Trap1)) {
				create_trap(0, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Trap2)) {
				create_trap(1, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Stone1)) {
				create_ground(0, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Stone2)) {
				create_ground(1, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Stone3)) {
				create_ground(2, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Stone4)) {
				create_ground(3, create_vec2(x * scaleFactor, y * scaleFactor));
			}
			else if (compare_colors(to_color(get_pixel_data(x, y)), Stone5)) {
				create_ground(4, create_vec2(x * scaleFactor, y * scaleFactor));
			}
		}
	}

	SDL_FreeSurface(maps);
}
 

void map_init(char* location)
{
	maps = IMG_Load(location);

	if (maps == NULL)
	{
		maps = SDL_CreateRGBSurface(0, 512, 32, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

		if (maps != NULL)
		{
			Rect rect = { 0,0,512,32 };
			SDL_FillRect(maps, &rect, 0xFFFFFFFF);
		}
		else
			printf("Error when creating maps surface!\n");
	}
	else
		printf("Successfull!\n");
}

void render_map() {

	//if (maps == NULL)
	//	return;

	for (int y = 0; y < maps->h; y++) {
		for (int x = 0; x < maps->w; x++) {
			
			int parameters[3] = { x,y, temp};

			Color color = to_color(get_pixel_data(x, y));
			DrawInteractiveRectangleOnScreen((Rect) {x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale}, color, put_pixel, parameters);
			
			if (compare_colors(color, Player))
				DrawClipImage(LoadTexture("resources/players/player.png", true, (Vector2) { 48, 48 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 48, 48 }, 0, false);
			else if(compare_colors(color, Bat))
				DrawClipImage(LoadTexture("resources/enemies/enemytwo.png", true, (Vector2) { 16, 16 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 16, 16 }, 0, false);
			else if (compare_colors(color, Archer))
				DrawClipImage(LoadTexture("resources/enemies/enemy3.png", true, (Vector2) { 32, 32 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 32, 32 }, 0, false);
			else if (compare_colors(color, Skeleton))
				DrawClipImage(LoadTexture("resources/enemies/enemy.png", true, (Vector2) { 43, 38 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 43, 38 }, 0, false);
			else if (compare_colors(color, Boss))
				DrawClipImage(LoadTexture("resources/enemies/boss.png", true, (Vector2) { 96, 96 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 96, 96 }, 0, false);
			else if (compare_colors(color, Stone2))
				DrawClipImage(LoadTexture("resources/environment/stone_ground_corner_left.png", true, (Vector2) { 417, 417 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 417, 417 }, 0, false);
			else if (compare_colors(color, Stone3))
				DrawClipImage(LoadTexture("resources/environment/stone_ground_corner_right.png", true, (Vector2) { 417, 417 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 417, 417 }, 0, false);
			else if (compare_colors(color, Stone1))
				DrawClipImage(LoadTexture("resources/environment/stone_ground.png", true, (Vector2) { 417, 417 }), (Rect) { x*scale + (camera->position.x - camera->width / 2), y*scale, scale, scale }, (Rect) { 0, 0, 417, 417 }, 0, false);
			else if (compare_colors(color, Stone4))
				DrawClipImage(LoadTexture("resources/environment/stone_ground_left.png", true, (Vector2) { 417, 417 }), (Rect) { x* scale + (camera->position.x - camera->width / 2), y* scale, scale, scale }, (Rect) { 0, 0, 417, 417 }, 0, false);
			else if (compare_colors(color, Stone5))
				DrawClipImage(LoadTexture("resources/environment/stone_ground_right.png", true, (Vector2) { 417, 417 }), (Rect) { x* scale + (camera->position.x - camera->width / 2), y* scale, scale, scale }, (Rect) { 0, 0, 417, 417 }, 0, false);
		}
	}
}


void onclick(int* object) {
	switch (object[0]) {
	case 0:
		temp = 0xFF1900FF; // Player
		break;
	case 1:
		temp = 0xFFFF0010; // Bat
		break;
	case 2:
		temp = 0xFFC1000C; // Skeleton
		break;
	case 3:
		temp = 0xFF820008; // Archer
		break;
	case 4:
		temp = 0xFF000000; // Boss
		break;
	case 5:
		temp = 0xFF9E5C38; // Trap1
		break;
	case 6:
		temp = 0xFF00821A; // Trap2
		break;
	case 7:
		temp = 0xFFFF00FA; // Dirt
		break;
	case 8:
		temp = 0xFFA300A0; // Grass
		break;
	case 9:
		temp = 0xFFBCBCBC; // Stone1
		break;
	case 10:
		temp = 0xFF9B9B9B; // Stone2
		break;
	case 11:
		temp = 0xFF777777; // Stone3
		break;
	case 12:
		temp = 0xFF606060; // Stone4
		break;
	case 13:
		temp = 0xFF3D3D3D; // Stone5
		break;
	case 14:
		temp = 0xFFFFFFFF; // Delete
		break;
	}
}

Uint32 get_pixel_data(int x, int y)
{
	if (maps == NULL || maps->format == NULL)
	{
		printf("error when getting pixel data!\n");
		return 0;
	}
		

	Lock(maps);
	int bpp = maps->format->BytesPerPixel;
	//printf("bpp: %d\n", bpp);
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

	//Unlock(maps);

	//Lock(maps);

	//SavePNG(maps, mapLocation);

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
			fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
			return;
		}
	}
}