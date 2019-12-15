#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"

#include "Utils.h"

#include "Collision.h"

#include "Audio.h"

#include "Time.h"

#include <Windows.h>


GameObject* local_player = NULL;

float gravity = 0.15f, max_y_speed = 4.0f;

float jump_force = 3.0f, player_speed = 200.0f, player_accel = 0.15f;

float on_ground_tolerance = 0.1f;


void attack(GameObject* self)
{
	float timer = 0;

	while (timer <= self->attack_time)
	{
		timer += deltaTime;

		float x_size = (self->collider.size.x) * self->transform->scale.x;
		float y_size = (self->collider.size.x) * self->transform->scale.y;

		float forward = self->transform->left ? -16 / (x_size + 4) : 1;

		BoxCollider boxcol;
		boxcol.size = (Vector2) { (x_size * 1.5f), y_size * 0.5f };
		boxcol.offset = (Vector2) { forward * (x_size + 4), y_size * 0.5f };

		bool to_break = false;

		GameObjectList list = GetInteractsOfCollider(boxcol, self->transform->position);
		for (int i = 0; i < list.Count; i++) {

			if (list.List[i] == NULL)
				continue;

			GameObject* go = list.List[i];

			if (go == self)
				continue;

			if (go->health > 0)
				go->health -= self->attack_force;

			to_break = true;

			printf("dmg to %X, left health: %d\n", go, go->health);
		}

		if (to_break)
			break;

		Sleep((int)(deltaTime * 1000));
	}
}

void localplayer_start(GameObject* self)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");

	local_player = self;

	self->velocity = (Vector2) { 0, 0 };
	self->extra_velocity = (Vector2) { 0, 0 };

	self->health = 100;

	self->attack_force = 10;
	self->attack_in_seconds = 0.6f;
	self->attack_time = 0.4f;

	self->dash_in_seconds = 1.0f;
	self->dash_force = 3.0f;


	self->attack_in_seconds_counter = 0;
	self->dash_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

void camera_position(GameObject* object)
{
	Point position;
	position.x = object->transform->position.x + object->image->clip_size.x;
	position.y = object->transform->position.y - object->image->clip_size.y / 2;

	camera->position = position;
}

//void dash(GameObject* self)
//{
//	float start_time = get_time();
//	float cur_time = start_time;
//
//	while (start_time + self->dash_time >= cur_time)
//	{
//		self->
//
//		Sleep(deltaTime);
//	}
//}

void localplayer_update(GameObject* self)
{
	DrawTextInGame("test", (Vector2) { 100, 400 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);

	float cur_time = get_time();

	if (self->health > 0) // alive
	{
		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		camera_position(self);

		GameObject* on_ground = is_on_platform(self, LAYER_GROUND, on_ground_tolerance);

		if (on_ground)
		{
			self->velocity.y = float_lerp(self->velocity.y, 0, deltaTime * 5.0f);

			if (keystate[SDL_SCANCODE_UP])
				self->velocity.y = -jump_force;

		}
		else // on air
		{
			if (self->velocity.y <= max_y_speed)
			{
				self->velocity.y += gravity;

				if(keystate[SDL_SCANCODE_DOWN])
					self->velocity.y += gravity;
			}
				
		}

		if (!(self->ignore_movement && self->ignore_movement_time > cur_time))
		{
			if (keystate[SDL_SCANCODE_X] && self->attack_in_seconds_counter <= cur_time) // fire
			{
				set_animator_state(self, "shoot", 0.45f, true);

				self->attack_in_seconds_counter = cur_time + self->attack_in_seconds;
			}

			if (keystate[SDL_SCANCODE_RIGHT])
			{
				self->transform->left = false;

				self->velocity.x = clamp(self->velocity.x + player_accel, -1.0f, 1.0f);

				set_animator_state(self, "run", 0, 0);
			}
			else if (keystate[SDL_SCANCODE_LEFT])
			{
				self->transform->left = true;

				self->velocity.x = clamp(self->velocity.x - player_accel, -1.0f, 1.0f);

				set_animator_state(self, "run", 0, 0);
			}
			else // no input
			{
				self->velocity.x = float_lerp(self->velocity.x, 0, player_accel);

				set_animator_state(self, "idle", 0, 0);
			}


			if (keystate[SDL_SCANCODE_Z] && self->dash_in_seconds_counter <= cur_time) // dash
			{
				//set_animator_state(self, "dash", 0.35f, true);

				self->extra_velocity.x = ( self->transform->left ? -1.0f : 1.0f ) * self->dash_force;

				self->dash_in_seconds_counter = cur_time + self->dash_in_seconds;
			}
		}
		else // no input
		{
			self->velocity.x = float_lerp(self->velocity.x, 0, player_accel);

			set_animator_state(self, "idle", 0, 0);
		}


		Point old_position = self->transform->position;
		self->transform->position = point_sum(self->transform->position, vec2_multiplier(vec2_sum((Point) { self->velocity.x, self->velocity.y }, self->extra_velocity), deltaTime * player_speed));

		//
		GameObjectList interact_list = GetInteracts(self);

		GameObject* later_on_ground = is_on_platform(self, LAYER_GROUND, on_ground_tolerance);

		for (int i = 0; i < interact_list.Count; i++)
		{
			GameObject* interacted_object = interact_list.List[i];

			if (interacted_object == NULL || interacted_object == later_on_ground) continue;

			Point new_position = self->transform->position;
			self->transform->position = old_position;

			Point diff = { new_position.x - old_position.x, new_position.y - old_position.y };
			
			if (collider_left_down(self).y <= collider_left_up(interacted_object).y)
			{
				self->transform->position = new_position;
				self->transform->position.y = collider_left_up(interacted_object).y - collider_size(self).y;
			}
			else
			{
				self->transform->position.y = new_position.y;
			}


			break;
		}
		//

		self->extra_velocity = vec2_movetowards(self->extra_velocity, (Vector2) { 0, 0 }, deltaTime * 5.0f);

	}
	else // dead
	{
		set_animator_state(self, "die", 0, false);
	}

}

void test_start(GameObject* self) {

	self->health = 20;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 20;
	self->attack_in_seconds = 0.7f;
	self->attack_time = 0.4f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}


void test_update(GameObject * self) {


}

void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/player.png", true, create_vec2(48, 48)); // local_player creation
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

		strcpy(animations[2].state_name, "shoot");
		{
			initialize_int_list(&animations[2].sprites);

			add_int_to_list(&animations[2].sprites, 16);
			add_int_to_list(&animations[2].sprites, 17);
			add_int_to_list(&animations[2].sprites, 18);
			add_int_to_list(&animations[2].sprites, 19);
			add_int_to_list(&animations[2].sprites, 20);

			animations[2].loop = false;

			animations[2].current_index = 0;
			animations[2].current_frame = 0;
			animations[2].wait_frame = 6;
		}

		strcpy(animations[3].state_name, "die");
		{
			initialize_int_list(&animations[3].sprites);

			add_int_to_list(&animations[3].sprites, 24);
			add_int_to_list(&animations[3].sprites, 25);
			add_int_to_list(&animations[3].sprites, 26);
			add_int_to_list(&animations[3].sprites, 27);
			add_int_to_list(&animations[3].sprites, 28);
			add_int_to_list(&animations[3].sprites, 29);
			add_int_to_list(&animations[3].sprites, 30);
			add_int_to_list(&animations[3].sprites, 31);


			animations[3].loop = true;

			animations[3].current_index = 0;
			animations[3].current_frame = 0;
			animations[3].wait_frame = 6;
		}

		GameObject* gameObject = GameObject_New(GameObjects.Count, spawn_position, spawn_scale, (BoxCollider) { 12, 4, 24, 35 }, LAYER_PLAYER, image, animations, animation_count, &localplayer_start, &localplayer_update); // spawning local player
	}

	image = LoadTexture("resources/environment/ground.png", false, (Vector2) { 0, 0 });

	GameObject_New(GameObjects.Count, create_point(0, 500), (Vector2) { 1, 1 }, (BoxCollider) { 0, 20, image->rect.w, image->rect.h - 20 }, LAYER_GROUND, image, NULL, 0, 0, 0); // ground


}

GameObjectList GetObjectsOfLayer(int layer)
{
	GameObjectList list;
	initialize_game_object_list(&list);

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* gameObject = GameObjects.List[i];

		if (gameObject->layer == layer)
			add_game_object_to_list(&list, gameObject);
	}

	return list;
}

void Render()
{
	// UI things


	////////////
	for (int i = (int)pow(2, sizeof(LAYERS) - 1); i != 1; i /= 2)
	{
		GameObjectList list = GetObjectsOfLayer(i);

		for (int j = 0; j < list.Count; j++)
		{
			GameObject* gameObject = list.List[j];

			if (gameObject != NULL)
			{
				GameObject_Update(gameObject);


				GameObject_Draw(gameObject);
			}
		}


	}

}