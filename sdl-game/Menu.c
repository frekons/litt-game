#include "Draws.h"

#include "Globals.h"

#include "Game.h"

#include "Editor.h"

#include "Colors.h"

#include "Utils.h"

InputBox MapInputBox = { "map1", false };

char current_state[256] = "";

void play() {
	
	char load_location[512];
	strcpy(load_location, "resources\\map\\");
	strcat(load_location, MapInputBox.buffer);
	strcat(load_location, ".png");

	if (file_exists(load_location))
	{
		strcpy(current_state, "");

		Start(); // don't use Game::GameObjects before this!

		game_state = INGAME;
	}
	else 
	{
		strcpy(current_state, "could not find the map!");
	}
}

void editor() {

	strcpy(current_state, "");

	char load_location[512];
	strcpy(load_location, "resources\\map\\");
	strcat(load_location, MapInputBox.buffer);
	strcat(load_location, ".png");

	InitializeEditor(load_location, MapInputBox.buffer);

	game_state = EDITOR;
}

void instructions() {

	strcpy(current_state, "");

	game_state = HOWTOPLAY;
}

void exit_game() {
	game_state = QUIT;
}

void RenderMenu() {

	DrawTextOnScreen(current_state, (Vector2) { camera->width / 2, camera->height / 2 - 60 * 2 }, White, Font_Minecraft);

	DrawInputBoxOnScreen("map_name", &MapInputBox, (Rect) { camera->width / 2 - 100, camera->height / 2 - 60, 200, 50 }, Black, White, Font_Minecraft);

	DrawButtonOnScreen("Oyna", (Rect) { camera->width / 2 - 100, camera->height / 2, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, play, NULL);
	DrawButtonOnScreen("Harita Editoru", (Rect) { camera->width / 2 - 100, camera->height / 2 + 60, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, editor, NULL);
	DrawButtonOnScreen("Nasil Oynanir?", (Rect) { camera->width / 2 - 100, camera->height / 2 + 60 * 2, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, instructions, NULL);
	DrawButtonOnScreen("Cikis", (Rect) { camera->width / 2 - 100, camera->height / 2 + 60 * 3, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, exit_game, NULL);
}

