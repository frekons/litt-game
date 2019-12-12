#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"


GameObject* local_player = NULL;

float jump_force = 4.0f, player_speed = 5.0f, player_accel = 12.0f;

Vector2 local_player_velocity = { 0,0 };

int wait_time = 0;

bool compare_animator_state(GameObject* self, char* state) // time is in seconds
{
	return strcmp(self->current_state, state) == 0;
}

void set_animator_state(GameObject* self, char* state, float time) // time is in seconds
{
	if (compare_animator_state(self, state))
		return;

	Uint32 current_time = SDL_GetTicks();

	if (wait_time > current_time) return;

	wait_time = current_time + time * 1000.0f;

	Animation* animation = get_animation_sprites(self, state);

	if (animation)
		animation->current_index = 0;

	strcpy(self->current_state, state);
}

float clamp(float value, float min, float max)
{
	if (value > max)
		return max;

	if (value < min)
		return min;

	return value;
}

void localplayer_start(GameObject* self)
{
	local_player = self;

	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");

	self->health = 100;
}

void localplayer_update(GameObject* self)
{
	//Uint32 current_time = SDL_GetTicks();

	//if (wait_time > current_time) return;

	Uint8* keystate = SDL_GetKeyboardState(NULL);
	
	if (keystate[SDL_SCANCODE_G])
	{
		set_animator_state(self, "die", 5.0f);
	}

	if (keystate[SDL_SCANCODE_F])
	{
		set_animator_state(self, "attack", 0.525f);
	}

	if (keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])
	{
		//if(is_on_platform(self))
			local_player_velocity.y = -jump_force;
	}

	if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT])
	{
		self->transform->left = false;

		local_player_velocity.x += deltaTime * player_accel;
		local_player_velocity.x = clamp(local_player_velocity.x, -1, 1);

		set_animator_state(self, "run", 0);
	}
	else if(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])
	{
		self->transform->left = true;

		local_player_velocity.x -= deltaTime * player_accel;
		local_player_velocity.x = clamp(local_player_velocity.x, -1, 1);


		set_animator_state(self, "run", 0);
	}
	else
	{
		local_player_velocity.x -= local_player_velocity.x * deltaTime * player_accel;

		set_animator_state(self, "idle", 0);
	}

	//if(!is_on_platform(self))
		//local_player_velocity.y += deltaTime * 5.0;
	//else
	//	local_player_velocity.y = 0;


	self->transform->position.x += local_player_velocity.x * player_speed;
	self->transform->position.y += local_player_velocity.y;
}


void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/spritePlayer.png");

	Point spawn_position = { 50,500 };
	Vector2 spawn_scale = { 0.5f, 0.5f };

	int animation_count = 4;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	strcpy(animations[0].state_name, "idle");
	{
		initialize_int_list(&animations[0].sprites);

		add_int_to_list(&animations[0].sprites, 0);
		add_int_to_list(&animations[0].sprites, 1);
		add_int_to_list(&animations[0].sprites, 2);
		add_int_to_list(&animations[0].sprites, 3);
		add_int_to_list(&animations[0].sprites, 4);

		animations[0].loop = true;

		animations[0].current_index = 0;
		animations[0].current_frame = 0;
		animations[0].wait_frame = 6;
	}

	strcpy(animations[1].state_name, "run");
	{
		initialize_int_list(&animations[1].sprites);

		add_int_to_list(&animations[1].sprites, 8);
		add_int_to_list(&animations[1].sprites, 9);
		add_int_to_list(&animations[1].sprites, 10);
		add_int_to_list(&animations[1].sprites, 11);
		add_int_to_list(&animations[1].sprites, 12);
		add_int_to_list(&animations[1].sprites, 13);
		add_int_to_list(&animations[1].sprites, 14);
		add_int_to_list(&animations[1].sprites, 15);

		animations[1].loop = true;

		animations[1].current_index = 0;
		animations[1].current_frame = 0;
		animations[1].wait_frame = 6;
	}

	strcpy(animations[2].state_name, "attack");
	{
		initialize_int_list(&animations[2].sprites);

		add_int_to_list(&animations[2].sprites, 16);
		add_int_to_list(&animations[2].sprites, 17);
		add_int_to_list(&animations[2].sprites, 18);
		add_int_to_list(&animations[2].sprites, 19);
		add_int_to_list(&animations[2].sprites, 20);
		add_int_to_list(&animations[2].sprites, 21);
		add_int_to_list(&animations[2].sprites, 22);

		animations[2].loop = true;

		animations[2].current_index = 0;
		animations[2].current_frame = 0;
		animations[2].wait_frame = 6;
	}

	strcpy(animations[3].state_name, "die");
	{
		initialize_int_list(&animations[3].sprites);

		add_int_to_list(&animations[3].sprites, 32);
		add_int_to_list(&animations[3].sprites, 33);
		add_int_to_list(&animations[3].sprites, 34);
		add_int_to_list(&animations[3].sprites, 35);
		add_int_to_list(&animations[3].sprites, 36);
		add_int_to_list(&animations[3].sprites, 37);
		add_int_to_list(&animations[3].sprites, 38);

		animations[3].loop = false;

		animations[3].current_index = 0;
		animations[3].current_frame = 0;
		animations[3].wait_frame = 6;
	}

	GameObject* gameObject = GameObject_New(spawn_position, spawn_scale, image, true, create_vec2(32,32), animations, animation_count, &localplayer_start, &localplayer_update); // spawning local player

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