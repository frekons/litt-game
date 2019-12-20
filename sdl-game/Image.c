#include "Image.h"

#include "Globals.h"

#include <SDL2/SDL_image.h>

#include "Structs.h"

PointerList image_list;

void InitializeImage()
{
	initialize_list(&image_list);
}

typedef struct {
	SDL_Texture* texture;
	char path[128];
	Rect rect;
}texture_cache;

texture_cache* find_image(char* path)
{

	for (int i = 0; i < image_list.Count; i++)
	{
		texture_cache* image = image_list.List[i];

		if (image == NULL)
			continue;

		if (strcmp(image->path, path) == 0)
			return image;
	}

	return NULL;
}

Image* LoadTexture(const char* path, bool sprite_sheet, Vector2 clip_size)
{
	texture_cache* cache_image = find_image(path);

	if (cache_image != NULL)
	{
		Image* image = (Image*)malloc(sizeof(Image));

		image->rect = cache_image->rect;
		image->texture = cache_image->texture;

		image->sprite_sheet = sprite_sheet;
		image->clip_size = clip_size;

		return image;
	}

	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	Rect rect;

	if (texture == NULL)
	{
		SDL_Log("Error when loading texture: '%s', error: %s\n", path, SDL_GetError());
		return NULL;
	}

	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	Image* image = (Image*)malloc(sizeof(Image));
	image->rect = rect;
	image->texture = texture;

	image->sprite_sheet = sprite_sheet;
	image->clip_size = clip_size;

	cache_image = (texture_cache*)malloc(sizeof(texture_cache));

	cache_image->texture = texture;
	memcpy(cache_image->path, path, sizeof(cache_image->path));
	cache_image->rect = rect;

	add_member_to_list(&image_list, cache_image);

	return image;
}

#include "Utils.h"

bool UnloadTexture(Image* img)
{
	//SDL_DestroyTexture(img->texture);

	free(img);

	//int index = delete_member_from_list(&image_list, img);

	//printf("index: %d\n", index);

	//return index >= 0;

	return true;
}


void DrawImage(Image* image, Rect rect, bool left)
{
	if (image == NULL) return;

	SDL_RenderCopyEx(renderer, image->texture, NULL, &rect, 0, NULL, left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void DrawClipImage(Image* image, Rect rect, Rect clipRect, int index, bool left)
{
	if (image == NULL) return;

	int x_count = image->rect.w / clipRect.w;
	int y_count = image->rect.h / clipRect.h;

	int x_offset = index % x_count;
	
	int y_offset = 0;
	while (index >= x_count)
	{
		index -= x_count;
		y_offset++;
	}


	clipRect.x = x_offset * clipRect.w;
	clipRect.y = y_offset * clipRect.h;

	

	SDL_RenderCopyEx(renderer, image->texture, &clipRect, &rect, 0, NULL, left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}


//SDL_Texture** SplitTexture(Image* texture, Vector2 split_size, int *size)
//{
//	SDL_Texture** SplitedTextures = NULL;
//
//	int count = 0;
//	for (int y = 0; y * split_size.y <= texture->rect.h; y++)
//	{
//		for (int x = 0; x * split_size.x <= texture->rect.w; x++)
//		{
//
//			SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, split_size.x, split_size.y);
//
//			Rect rect, rect2;
//			rect.x = x * split_size.x;
//			rect.y = y * split_size.y;
//
//			rect.w = split_size.x;
//			rect.h = split_size.y;
//
//			rect2.x = 0;
//			rect2.y = 0;
//
//			rect2.w = split_size.x;
//			rect2.h = split_size.y;
//
//			 (tex, texture->texture, &rect, NULL);
//
//			count++;
//			SplitedTextures = (Image**)realloc(SplitedTextures, count * sizeof(Image*));
//			SplitedTextures[count - 1] = tex;
//
//			//SplitedTextures[count - 1]->texture = tex;
//			//SplitedTextures[count - 1]->rect = rect2;
//		}
//
//	}
//	
//	if (size != NULL)
//		*size = count;
//
//	return SplitedTextures;
//}