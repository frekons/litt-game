#include "Draws.h"

#include "Globals.h"

#include "GameObject.h"

#include "Utils.h"

#include "Collision.h"

#include "Image.h"

#include "Colors.h"

void InitializeDraws()
{
	TTF_Init();

	if (texture_list_to_be_destroyed.List != NULL)
	{
		for (int i = 0; i < texture_list_to_be_destroyed.Count; i++)
			free(texture_list_to_be_destroyed.List[i]);

		free(texture_list_to_be_destroyed.List);
	}
	
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

void DrawInteractiveRectangleOnScreen(Rect rect, Color color, void *onClick, void *parameters)
{
	DrawFilledRectangleOnScreen(rect, color);

	if ((SDL_BUTTON_LMASK & mouse_button_mask) && IsInteractingRect(rect, mouse_position))
	{
		if (onClick)
		{
			typedef void func(void *);
			func *f = (func *)onClick;
			f(parameters);

			//mouse_button_mask = 0;
		}
	}
}

void DrawInteractiveRectangleWithImageOnScreen(char *img_directory, Rect rect, Color color, void *onClick, void *parameters)
{
	Image *image = LoadTexture(img_directory, false, (Vector2){0, 0});
	DrawImage(image, rect, false);

	UnloadTexture(image);

	if ((SDL_BUTTON_LMASK & mouse_button_mask) && IsInteractingRect(rect, mouse_position))
	{
		if (onClick)
		{
			typedef void func(void *);
			func *f = (func *)onClick;
			f(parameters);

			mouse_button_mask = 0;
		}
	}
}

void DrawFilledRectangleOnScreen(Rect rect, Color color)
{
	//SDL_Surface* pSurface = SDL_GetWindowSurface(game_window);

	//SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));

	if (color.a == 0) {
		return;
	}

	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderFillRect(renderer, &rect);

	set_render_color(temp_color);

	//SDL_FreeSurface(pSurface);
}

void DrawFilledRectangleInGame(Rect rect, Color color)
{
	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderFillRect(renderer, &rect);

	set_render_color(temp_color);

	//SDL_Surface* pSurface = pSurface = SDL_GetWindowSurface(game_window);

	//SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));

	//SDL_FreeSurface(pSurface);
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

SDL_Texture *DrawTextOnScreen(char *str, Vector2 position, Color color, TTF_Font *font)
{

	SDL_Color sdl_color = {color.r, color.g, color.b, color.a};

	SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, str, sdl_color);

	if (surfaceMessage == NULL)
		return NULL;

	SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Rect rect;

	rect.w = surfaceMessage->w;
	rect.h = surfaceMessage->h;

	rect.x = position.x - rect.w / 2;
	rect.y = position.y - rect.h / 2;

	SDL_RenderCopy(renderer, Message, NULL, &rect);

	SDL_FreeSurface(surfaceMessage);
	//SDL_DestroyTexture(Message);
	add_member_to_list(&ToBeDestroyedTextures, Message);

	//add_member_to_list(&texture_list_to_be_destroyed, Message);

	return Message;
}

SDL_Texture *DrawTextInGame(char *str, Vector2 position, Color color, TTF_Font *font)
{

	SDL_Color sdl_color = {color.r, color.g, color.b, color.a};

	SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, str, sdl_color);

	if (surfaceMessage == NULL)
		return NULL;

	SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	Rect rect;

	rect.x = position.x;
	rect.y = position.y;

	rect.w = surfaceMessage->w;
	rect.h = surfaceMessage->h;

	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	rect.x -= rect.w / 2;
	rect.y -= rect.h / 2;

	SDL_RenderCopy(renderer, Message, NULL, &rect);

	SDL_FreeSurface(surfaceMessage);
	//SDL_DestroyTexture(Message);
	add_member_to_list(&ToBeDestroyedTextures, Message);

	//add_member_to_list(&texture_list_to_be_destroyed, Message);

	return Message;
}

void DrawInputBoxOnScreen(char *str, InputBox* inputBox, Rect rect, Color color, Color text_color, TTF_Font *font)
{
	DrawFilledRectangleOnScreen(rect, color);
	 
	if (inputBox->buffer[0] == NULL)
		DrawTextOnScreen(str, (Vector2) { rect.x + rect.w / 2, rect.y + rect.h / 2 }, text_color, Font_Minecraft);
	else
		DrawTextOnScreen(inputBox->buffer, (Vector2) { rect.x + rect.w / 2, rect.y + rect.h / 2 }, text_color, Font_Minecraft);

	if ((SDL_BUTTON_LMASK & mouse_button_mask))
	{
		if (IsInteractingRect(rect, mouse_position))
		{
			inputBox->focus = true;

			mouse_button_mask = 0;
		}
		else 
		{
			inputBox->focus = false;
		}
	}

	if (inputBox->focus)
	{
		DrawRectangleOnScreen(rect, Red);

		int i;
		Uint8* keys = SDL_GetKeyboardState(NULL);
		for (i = 0; i < SDL_NUM_SCANCODES; i++)
		{
			if (keys[i])
			{
				int key_code = SDL_GetKeyFromScancode(i);

				if (key_code > 2048)
					continue;

				//printf("pressed key: %d\n", key_code);

				if (key_code == 8) // remove key
				{
					int str_len = strlen(inputBox->buffer);

					if (str_len < 1)
						str_len = 1;

					inputBox->buffer[str_len - 1] = '\0';

					keys[i] = NULL;

					continue;
				}

				char str[2] = { key_code, '\0' };
				strcat(inputBox->buffer, str);

				keys[i] = NULL;
			}
		}
	}
		
}

void DrawButtonOnScreen(char *str, Rect rect, Color color, Color text_color, TTF_Font *font, void *onClick, void *parameters)
{
	DrawFilledRectangleOnScreen(rect, color);

	DrawTextOnScreen(str, (Vector2){rect.x + rect.w / 2, rect.y + rect.h / 2}, text_color, Font_Minecraft);

	if ((SDL_BUTTON_LMASK & mouse_button_mask) && IsInteractingRect(rect, mouse_position))
	{
		if (onClick)
		{
			typedef void func(void *);
			func *f = (func *)onClick;
			f(parameters);

			mouse_button_mask = 0;
		}
	}
}

void DrawButtonWithImageOnScreen(char *str, char *img_directory, Rect rect, Color color, Color text_color, TTF_Font *font, void *onClick, void* parameters)
{
	//DrawFilledRectangleOnScreen(rect, color);
	Image *image = LoadTexture(img_directory, false, (Vector2){0, 0});
	DrawImage(image, rect, false);

	UnloadTexture(image);

	DrawTextOnScreen(str, (Vector2){rect.x + rect.w / 2, rect.y + rect.h / 2}, text_color, Font_Minecraft);

	if ((SDL_BUTTON_LMASK & mouse_button_mask) && IsInteractingRect(rect, mouse_position))
	{
		if (onClick)
		{
			typedef void func(void*);
			func *f = (func *)onClick;
			f(parameters);

			mouse_button_mask = 0;
		}
	}
}

void DrawButtonInGame(char *str, Rect rect, Color color, Color text_color, TTF_Font *font, void *onClick)
{
	DrawFilledRectangleInGame(rect, color);

	DrawTextInGame(str, (Vector2){rect.x + rect.w / 2, rect.y + rect.h / 2}, text_color, Font_Minecraft);

	rect.x -= (camera->position.x - camera->width / 2);
	rect.y -= (camera->position.y - camera->height / 2);

	if ((SDL_BUTTON_LMASK & mouse_button_mask) && IsInteractingRect(rect, mouse_position))
	{
		if (onClick)
		{
			typedef void func(void);
			func *f = (func *)onClick;
			f();

			mouse_button_mask = 0;
		}
	}
}
