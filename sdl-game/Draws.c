#include "Draws.h"

#include "Globals.h"

#include "GameObject.h"

void InitializeDraws()
{
	TTF_Init();

	initialize_list(&texture_list_to_be_destroyed);

	Font_Minecraft = TTF_OpenFont("resources/fonts/Minecraft.ttf", 24);

	if (!Font_Minecraft)
		printf("Error on initializing font, error: %s\n", TTF_GetError());
}

void DrawRectangleOnScreen(Rect rect, Color color) 
{
	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderDrawRect(renderer, &rect);

	set_render_color(temp_color);
	
}


void DrawRectangleInGame(Rect rect, Color color)
{
	Color temp_color = get_render_color();

	set_render_color(color);

	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	SDL_RenderDrawRect(renderer, &rect);

	set_render_color(temp_color);

}


void DrawFilledRectangleOnScreen(Rect rect, Color color)
{
	SDL_Surface* pSurface = pSurface = SDL_GetWindowSurface(game_window);
	
	SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));

	SDL_FreeSurface(pSurface);
}

void DrawFilledRectangleInGame(Rect rect, Color color)
{
	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	SDL_Surface* pSurface = pSurface = SDL_GetWindowSurface(game_window);

	SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));

	SDL_FreeSurface(pSurface);

}

void DrawLineOnScreen(Point A, Point B, Color color)
{
	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderDrawLine(renderer, A.x, A.y, B.x, B.y);

	set_render_color(temp_color);
}

void DrawLineInGame(Point A, Point B, Color color)
{
	Color temp_color = get_render_color();

	set_render_color(color);

	A.x -= (camera->position.x - camera->width / 2);
	A.y -= (camera->position.y - camera->height / 2);

	B.x -= (camera->position.x - camera->width / 2);
	B.y -= (camera->position.y - camera->height / 2);

	SDL_RenderDrawLine(renderer, A.x, A.y, B.x, B.y);

	set_render_color(temp_color);
}


SDL_Texture* DrawTextOnScreen(char* str, Vector2 position, Color color, TTF_Font* font)
{
	SDL_Color sdl_color = { color.r, color.g, color.b, color.a };  

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, str, sdl_color); 

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Rect rect;

	rect.x = position.x;
	rect.y = position.y;

	rect.w = surfaceMessage->w;
	rect.h = surfaceMessage->h;

	SDL_RenderCopy(renderer, Message, NULL, &rect); 

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

	//add_member_to_list(&texture_list_to_be_destroyed, Message);

	return Message;
}


SDL_Texture* DrawTextInGame(char* str, Vector2 position, Color color, TTF_Font* font)
{
	SDL_Color sdl_color = { color.r, color.g, color.b, color.a };

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, str, sdl_color);

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Rect rect;

	rect.x = position.x;
	rect.y = position.y;

	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	rect.w = surfaceMessage->w;
	rect.h = surfaceMessage->h;

	SDL_RenderCopy(renderer, Message, NULL, &rect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

	//add_member_to_list(&texture_list_to_be_destroyed, Message);

	return Message;
}

