#include "Image.h"

#include "Globals.h"

#include <SDL2/SDL_image.h>


Image* LoadTexture(const char* path)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	Rect rect;

	if (texture != NULL)
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

		Image* image = (Image*)malloc(sizeof(Image));
		image->rect = rect;
		image->texture = texture;

		return image;
	}
	

	return NULL;
}


void DrawImage(Image* image, Rect rect)
{
	if (image == NULL) return;

	SDL_RenderCopy(renderer, image->texture, NULL, &rect);
}