#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"

#include "Utils.h"

#include "Collision.h"

#include "Audio.h"


GameObject* local_player = NULL;

float gravity = 20.0f;

float jump_force = 800.0f, player_speed = 5.0f, player_accel = 12.0f;

Vector2 local_player_velocity = { 0,0 };

int wait_time = 0;

bool ignore_movement;

bool compare_animator_state(GameObject* self, char* state) // time is in seconds
{
	return strcmp(self->current_state, state) == 0;
}

void set_animator_state(GameObject* self, char* state, float time, bool ignore_move) // time is in seconds
{
	if (compare_animator_state(self, state))
		return;

	if (ignore_move) // logical
		ignore_movement = true;

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

bool float_compare(float one, float two, float tolerance)
{
	if (one > two - tolerance && one < two + tolerance)
		return true;

	return false;
}

GameObject* is_on_platform(GameObject* self)
{

	GameObjectList list = GetInteracts(self);

	for (int i = 0; i < list.Count; i++)
		if (LAYER_GROUND == list.List[i]->layer)
		{
			float foot_y = self->transform->position.y + self->image->clip_size.y * self->transform->scale.y;

			float ground_y = list.List[i]->transform->position.y + list.List[i]->collider.offset.y;

			if (float_compare(foot_y, ground_y, 10)) // eðer ayaðýn platformun üstündeyse
			{
				self->transform->position.y = ground_y - self->image->clip_size.y * self->transform->scale.y;

				return list.List[i];
			}
				
		}
			

		

	return NULL;
}


bool is_collides_except(GameObject* self, GameObject* except)
{
	GameObjectList list = GetInteracts(self);

	bool result = false;

	for (int i = 0; i < list.Count; i++)
	{
		if (list.List[i] == except)
			return false;

		result = true;
		break;
	}
	

	return result;
}

void localplayer_start(GameObject* self)
{
	local_player = self;

	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");

	self->health = 100;
}

Uint32 attack_cooldown = 700, attack_timer = 0;


void localplayer_update(GameObject* self)
{
	GameObject* on_ground = is_on_platform(self);

	if (!on_ground)
		local_player_velocity.y += deltaTime * gravity;
	else
	{
		local_player_velocity.y = 0;
	}
		

	Uint32 current_time = SDL_GetTicks();

	if (ignore_movement && wait_time > current_time) return;

	ignore_movement = false;

	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	
	//if (keystate[SDL_SCANCODE_G])
	//{
	//	set_animator_state(self, "die", 5.0f, true);
	//}

	if ( keystate[SDL_SCANCODE_F] && attack_timer <= current_time )
	{
		set_animator_state(self, "attack", 0.525f, false);

		play_sound("resources/sounds/attack.wav");

		attack_timer = current_time + attack_cooldown;
		float forward = self->transform->left ? -12/68.0f : 1;

		BoxCollider boxcol;
		boxcol.size =(Vector2){ 20,20 };
		boxcol.offset =(Vector2){ forward * 68,32 };



		GameObjectList LIST = GetInteractsOfCollider(boxcol, self->transform->position);
		for (int i = 0; i < LIST.Count; i++) {
			GameObject* go = LIST.List[i];
			if (go == self)
				continue;

			if (go->health > 0)
				go->health -=5;
			printf("dmg to %X\n", go);
		}

		// TO DO, look for enemy and damage it
	}

	if (keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])
	{
		if (on_ground)
		{
			local_player_velocity.y = -jump_force * deltaTime;

			play_sound("resources/sounds/jump.wav");
		}
			
	}

	if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT])
	{
		self->transform->left = false;

		local_player_velocity.x += deltaTime * player_accel;
		local_player_velocity.x = clamp(local_player_velocity.x, -1, 1);

		set_animator_state(self, "run", 0, false);
	}
	else if(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])
	{
		self->transform->left = true;

		local_player_velocity.x -= deltaTime * player_accel;
		local_player_velocity.x = clamp(local_player_velocity.x, -1, 1);


		set_animator_state(self, "run", 0, false);
	}
	else
	{
		local_player_velocity.x -= local_player_velocity.x * deltaTime * player_accel;

		set_animator_state(self, "idle", 0, false);
	}

	Point camera_pos;
	camera_pos.x = self->transform->position.x + self->image->clip_size.x * 2 + local_player_velocity.x * 150.0f;
	camera_pos.y = self->transform->position.y - self->image->clip_size.y / 2 + local_player_velocity.y - 75;

	camera->position = vec2_lerp(camera->position, camera_pos, 0.12f); // deltaTime should be used? (deltaTime causes screen shakes)

	self->transform->position.x += local_player_velocity.x * player_speed;
	self->transform->position.y += local_player_velocity.y;

	GameObjectList list = GetInteractsExceptLayer(self, LAYER_GROUND);

	GameObject* later_on_ground = is_on_platform(self);

	if (list.Count > 0 || (!later_on_ground && GetInteracts(self).Count > 0))
	{
		self->transform->position.x -= local_player_velocity.x * player_speed;
	}

	if (!on_ground && later_on_ground)
	{
		play_sound("resources/sounds/landing.wav");
	}

}

void test_start(GameObject* self) {

	self->health = 20;
	


}

void test_update(GameObject * self) {
	if (self->health > 0) {
		float diff_x = local_player->transform->position.x - self->transform->position.x;


		self->transform->position.x += diff_x * deltaTime * 2;
		set_animator_state(self, "run", 0, false);
		if (diff_x < 0) {
			self->transform->left = true;

		}
		else
			self->transform->left = false;
	}


	if (self->health <= 0)
		set_animator_state(self, "die", 0, false);

}

void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/spritePlayer.png", true, create_vec2(32, 32)); // local_player creation
	{
		Point spawn_position = { 50,400 };
		Vector2 spawn_scale = { 2.0f, 2.0f };

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


		GameObject* gameObject = GameObject_New(spawn_position, spawn_scale, (BoxCollider) { image->clip_size.x / 2, 0, image->clip_size.x/2, image->clip_size.y }, LAYER_PLAYER, image, animations, animation_count, &localplayer_start, &localplayer_update); // spawning local player
	}

	image = LoadTexture("resources/environment/ground.png", false, (Vector2) { 0, 0 });

	GameObject_New(create_point(0, 500), (Vector2) { 1, 1 }, (BoxCollider) { 0, 20, image->rect.w, image->rect.h - 20 }, LAYER_GROUND, image, NULL, 0, 0, 0); // ground

	image = LoadTexture("resources/enemies/enemy3.png", true, (Vector2) { 32, 32 });
	{
		Point spawn_position = { 50,456 };
		Vector2 spawn_scale = { 2.0f, 2.0f };

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
			add_int_to_list(&animations[0].sprites, 5);
			add_int_to_list(&animations[0].sprites, 6);
			add_int_to_list(&animations[0].sprites, 7);
			add_int_to_list(&animations[0].sprites, 8);
			add_int_to_list(&animations[0].sprites, 9);
			add_int_to_list(&animations[0].sprites, 10);
			add_int_to_list(&animations[0].sprites, 11);
			add_int_to_list(&animations[0].sprites, 12);
			animations[0].loop = true;

			animations[0].current_index = 0;
			animations[0].current_frame = 0;
			animations[0].wait_frame = 6;
		}

		strcpy(animations[1].state_name, "run");
		{
			initialize_int_list(&animations[1].sprites);

			add_int_to_list(&animations[1].sprites, 13);
			add_int_to_list(&animations[1].sprites, 14);
			add_int_to_list(&animations[1].sprites, 15);
			add_int_to_list(&animations[1].sprites, 16);
			add_int_to_list(&animations[1].sprites, 17);
			add_int_to_list(&animations[1].sprites, 18);
			add_int_to_list(&animations[1].sprites, 19);
			add_int_to_list(&animations[1].sprites, 20);

			animations[1].loop = true;

			animations[1].current_index = 0;
			animations[1].current_frame = 0;
			animations[1].wait_frame = 6;
		}

		strcpy(animations[2].state_name, "attack");
		{
			initialize_int_list(&animations[2].sprites);

			add_int_to_list(&animations[2].sprites, 39);
			add_int_to_list(&animations[2].sprites, 40);
			add_int_to_list(&animations[2].sprites, 41);
			add_int_to_list(&animations[2].sprites, 42);	
			add_int_to_list(&animations[2].sprites, 43);
			add_int_to_list(&animations[2].sprites, 44);
			add_int_to_list(&animations[2].sprites, 45);

			animations[2].loop = true;

			animations[2].current_index = 0;
			animations[2].current_frame = 0;
			animations[2].wait_frame = 6;
		}

		strcpy(animations[3].state_name, "die");
		{
			initialize_int_list(&animations[3].sprites);

			add_int_to_list(&animations[3].sprites, 91);
			add_int_to_list(&animations[3].sprites, 92);
			add_int_to_list(&animations[3].sprites, 93);
			add_int_to_list(&animations[3].sprites, 94);
			add_int_to_list(&animations[3].sprites, 95);
			add_int_to_list(&animations[3].sprites, 96);
			add_int_to_list(&animations[3].sprites, 97);

			animations[3].loop = false;

			animations[3].current_index = 0;
			animations[3].current_frame = 0;
			animations[3].wait_frame = 6;
		}


		GameObject* gameObject = GameObject_New(spawn_position, spawn_scale, (BoxCollider) {14,17,15,22}, LAYER_ENEMY, image, animations, animation_count, &test_start, &test_update); // spawning local player
	}


}



void Render()
{
	// UI things




	////////////
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