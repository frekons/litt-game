#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"

#include <time.h>

#include <Windows.h>


void localplayer_start(GameObject* self)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");
}

void localplayer_update(GameObject* self)
{
	//self->transform->position.x += 100.0f * deltaTime;

	if (GetAsyncKeyState('A') || GetAsyncKeyState('D'))
		strcpy(self->current_state, "run");
	else
		strcpy(self->current_state, "idle");
		

}


void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/spritePlayer.png");

	Point spawn_position = { 50,50 };
	Vector2 spawn_scale = { 1, 1 };

	Animation* animations = (Animation*)malloc(sizeof(Animation) * 4);

	strcpy(animations[0].state_name, "idle");

	initialize_int_list(&animations[0].sprites);

	add_int_to_list(&animations[0].sprites, 0);
	add_int_to_list(&animations[0].sprites, 1);
	add_int_to_list(&animations[0].sprites, 2);
	add_int_to_list(&animations[0].sprites, 3);
	add_int_to_list(&animations[0].sprites, 4);

	animations[0].current_index = 0;
	animations[0].current_frame = 0;
	animations[0].wait_frame = 6;

	strcpy(animations[1].state_name, "run");

	initialize_int_list(&animations[1].sprites);

	add_int_to_list(&animations[1].sprites, 8);
	add_int_to_list(&animations[1].sprites, 9);
	add_int_to_list(&animations[1].sprites, 10);
	add_int_to_list(&animations[1].sprites, 11);
	add_int_to_list(&animations[1].sprites, 12);
	add_int_to_list(&animations[1].sprites, 13);
	add_int_to_list(&animations[1].sprites, 14);
	add_int_to_list(&animations[1].sprites, 15);

	animations[1].current_index = 0;
	animations[1].current_frame = 0;
	animations[1].wait_frame = 6;

	GameObject* gameObject = GameObject_New(spawn_position, spawn_scale, image, true, create_vec2(32,32), animations, 2, &localplayer_start, &localplayer_update); // spawning local player

}

int size = 0;
Image* image;

int next = 0;
const int fps = 12;
int fps_sayac = 0;

int x = 100;

void Render()
{

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* gameObject = GameObjects.List[i];

		if (gameObject != NULL)
		{
			GameObject_Update(gameObject);

			GameObject_Draw(gameObject);
		}

	}



	//if (size == 0)
	//{
	//	image = LoadTexture("resources/players/spritePlayer.png");

	//	size = 8;
	//}

	//DrawClipImage(image, create_rect(x+=2, 200, 120, 170), create_rect(0, 0, 60, 85), next);
	//fps_sayac++;
	//if (fps_sayac >= fps)
	//{
	//	next++;
	//	fps_sayac = 0;
	//}

	//if (next >= size - 1)
	//	next = 0;
}