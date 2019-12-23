#include "Draws.h"

#include "Globals.h"

void play() {
	game_state = INGAME;
}

void editor() {
	game_state = EDITOR;
}

void instructions() {
	game_state = MENU;
}

void exit_game() {
	game_state = QUIT;
}

void RenderMenu() {
	DrawButtonOnScreen("Oyna", (Rect) { 300, 200, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, play, NULL);
	DrawButtonOnScreen("Harita Editoru", (Rect) { 300, 260, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, editor, NULL);
	DrawButtonOnScreen("Nasil Oynanir?", (Rect) { 300, 320, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, instructions, NULL);
	DrawButtonOnScreen("Cikis", (Rect) { 300, 380, 200, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, exit_game, NULL);
}

