#pragma once

#include <SDL2/SDL.h>

#include "Structs.h"

/////////////////////////////////////////////////////

//The SDL_Surface to hold the map data.
Surface* map;

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
GameObjectList InitializeEditor();

/**
*	Initializes the map
*/
void map_init();

/**
*	Processes the incoming pixel data 
*	and adds it into the GameObjectList stream
*/
void process_pixels();

/**
*	Puts a pixel on the map.png, 
*	which determines the coordinates of the objects.
*/
void put_pixel(int x, int y, Uint32 pixel);

/**
*	Gets the object color as red, green, blue;
*	turns it into a Uint32
*/
Uint32 get_object_color(Uint8 red, Uint8 green, Uint8 blue);

void onclick();

/**
*	For a easier access to get rgba.
*/
void get_rgba(Uint32 pixel_data, Uint8* rgba);