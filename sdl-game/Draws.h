#pragma once

//#ifndef DRAWS_H
//#define DRAWS_H

#include "Structs.h"

#include <SDL2/SDL_ttf.h>

void InitializeDraws();

void DrawRectangleOnScreen(Rect rect, Color color);
void DrawRectangleInGame(Rect rect, Color color);

void DrawInteractiveRectangleOnScreen(Rect rect, Color color, void* onClick, void* parameters);

void DrawFilledRectangleOnScreen(Rect rect, Color color);
void DrawFilledRectangleInGame(Rect rect, Color color);

void DrawLineOnScreen(Point A, Point B, Color color);
void DrawLineInGame(Point A, Point B, Color color);

SDL_Texture* DrawTextOnScreen(char* str, Vector2 position, Color color, TTF_Font* font);
SDL_Texture* DrawTextInGame(char* str, Vector2 position, Color color, TTF_Font* font);

void DrawButtonOnScreen(char* str, Rect rect, Color color, Color text_color, TTF_Font* font, void* onClick, void* parameters);
void DrawButtonWithImageOnScreen(char* str, char* img_directory, Rect rect, Color color, Color text_color, TTF_Font* font, void* onClick, void* parameters);

void DrawButtonInGame(char* str, Rect rect, Color color, Color text_color, TTF_Font* font, void* onClick);

TTF_Font* Font_Minecraft;

PointerList texture_list_to_be_destroyed;

//#endif // !DRAWS_H
