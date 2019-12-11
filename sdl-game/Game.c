#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"


void localplayer_start(GameObject* self)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");
}

void localplayer_update(GameObject* self)
{
	
}


void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/player.png");

	Point spawn_position = { 50,50 };
	Vector2 spawn_scale = { 1,1 };

	GameObject* gameObject = GameObject_New( spawn_position , spawn_scale, image, &localplayer_start, &localplayer_update); // spawning local player

}



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

}