#include "Draws.h"

#include "Globals.h"

#include "Game.h"

#include "Utils.h"


void continueToGame() {
	game_state = INGAME;
}


void back_to_mainmenu() {

	game_state = MENU;

	for (int i = 0; i < GameObjects.Count; i++)
	{
		destroy_object(GameObjects.List[i], 1);
		delete_game_object_at(&GameObjects, 0);
	}
}

void RenderPauseMenu() {
	DrawButtonOnScreen("Devam et", (Rect) { camera->width / 2 - 100, camera->height / 2, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, continueToGame, NULL);
	DrawButtonOnScreen("Anamenuye don", (Rect) { camera->width / 2 - 100, camera->height / 2 + 60 * 1, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, back_to_mainmenu, NULL);
}

