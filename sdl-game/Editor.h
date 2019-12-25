#pragma once

#include <SDL2/SDL.h>

#include "Structs.h"

/////////////////////////////////////////////////////

//The SDL_Surface to hold the map data.
Surface* maps;

//Where the game objects will be stored
GameObjectList list;

////////////////////////////////////////////////////

/**
*	Renders the editor menu
*/
void RenderEditor();

/**
*	Visualizes the current state of the map
*/
void render_map();

/**
*	Does all the initializations for editor.
*/
void InitializeEditor(char* location);

/**
	Returns an Pointer with all the gameobjects that are in the map
*/
void SaveMap();

void InitializeMap(char* map_location);

/**
*	Initializes the map
*/
void map_init(char* location);

/**
*	Puts a pixel on the map.png, 
*	which determines the coordinates of the objects.
*/
void put_pixel(int* parameters); // int x, int y, Uint32 pixel

/**
*	Gets the object color as red, green, blue;
*	turns it into a Uint32
*/
Uint32 get_object_color(Uint8 red, Uint8 green, Uint8 blue);

/**
	Get the pixel data of the pixel at x,y
*/
Uint32 get_pixel_data(int x, int y);

void onclick(Uint8 object);

//Easier access for locking and unlocking surfaces
void Unlock(SDL_Surface* surface);
void Lock(SDL_Surface* surface);

/**
Turns Uint32 to Color
*/
Color to_color(Uint32 pixel_data);