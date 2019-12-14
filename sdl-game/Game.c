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

float gravity = 20.0f;

float jump_force = 1000.0f, player_speed = 4.0f, player_accel = 12.0f;


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

	self->health = 100;

	self->attack_force = 10;
	self->attack_in_seconds = 0.7f;
	self->attack_time = 0.4f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}


void localplayer_update(GameObject* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	GameObject* on_ground = is_on_platform(self, LAYER_GROUND, 16);

	if (self->health > 0)
	{
		if (!on_ground)
		{
			if (self->velocity.y <= 15.0f)
				self->velocity.y += deltaTime * gravity;


			if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
				self->velocity.y += deltaTime * gravity;
		}
		else
		{
			self->velocity.y = 0;
		}
	}
	else {
		if (self->velocity.y <= 15.0f)
			self->velocity.y += deltaTime * gravity;
	}


		

	float current_time = get_time();

	if (!(self->ignore_movement && self->ignore_movement_time > current_time) && self->health > 0) 
	{


		//if ( keystate[SDL_SCANCODE_F] && self->attack_in_seconds_counter <= current_time )
		//{
		//	set_animator_state(self, "attack", 0.525f, false);

		//	play_sound("resources/sounds/attack.wav");

		//	self->attack_in_seconds_counter = current_time + self->attack_in_seconds;

		//	create_thread(attack, self);
		//	// TO DO, look for enemy and damage it
		//}

		self->animations.List[1]->wait_frame = 1.0f / fabs(self->velocity.x) * 15.0f; // run animation speed


		if (keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])
		{
			if (on_ground)
			{
				self->velocity.y = -jump_force * deltaTime;

				play_sound("resources/sounds/jump.wav");
			}

		}

		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT])
		{
			self->transform->left = false;

			self->velocity.x += deltaTime * player_accel * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f);
			self->velocity.x = clamp(self->velocity.x, -player_speed * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f), player_speed * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f));

			set_animator_state(self, "run", 0, false);
		}
		else if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])
		{
			self->transform->left = true;

			self->velocity.x -= deltaTime * player_accel * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f);
			self->velocity.x = clamp(self->velocity.x, -player_speed * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f), player_speed * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f));


			set_animator_state(self, "run", 0, false);
		}
		else
		{
			self->velocity.x -= self->velocity.x * deltaTime * player_accel * (keystate[SDL_SCANCODE_Z] ? 1.6f : 1.0f);

			set_animator_state(self, "idle", 0, false);
		}

		self->ignore_movement = false;
		
	}

	
	


	if (self->health > 0)
	{
		Point camera_pos;
		camera_pos.x = self->transform->position.x + self->image->clip_size.x * 2 + (self->velocity.x) * 150.0f / player_speed;
		camera_pos.y = self->transform->position.y - self->image->clip_size.y / 2 + (self->velocity.y) - 75;

		camera->position = point_lerp(camera->position, camera_pos, 0.12f); // deltaTime should be used? (deltaTime causes screen shakes)
	}


	if (self->health > 0)
	{
		GameObjectList enemy_list;
		if ((enemy_list = GetInteractsOnlyLayer(self, LAYER_ENEMY)).Count > 0)
		{
			Vector2 mine_center = vec2_sum(self->transform->position, collider_center(self->collider));
			Vector2 enemy_center = vec2_sum(enemy_list.List[0]->transform->position, collider_center(enemy_list.List[0]->collider));

			float speed = sign(mine_center.x - enemy_center.x);
			self->velocity.x += speed * (fabs(enemy_list.List[0]->velocity.x) > 1.0f ? fabs(enemy_list.List[0]->velocity.x) : 1.0f) * deltaTime * 2.75f;
			//self->velocity.y = speed * deltaTime * -10.0f;
		}
	}


	if (!on_ground)
	{
		if (self->velocity.y > 0)
			set_animator_state(self, "fall-down", 0, false);
		else
			set_animator_state(self, "jump", 0, false);
	}

	if (self->health > 0)
	{
		GameObject* on_head_of_enemy = is_on_something(self, LAYER_ENEMY, 0.01f);

		if (on_head_of_enemy && self->velocity.y > 0 && on_head_of_enemy->health > 0)
		{
			on_head_of_enemy->health = 0;

			self->velocity.y = -5.0f;

			play_sound("resources/sounds/kill.wav");
		}
	}


	if (self->health <= 0)
	{
		if (set_animator_state(self, "die", 10, true))
		{
			self->velocity.y = -10;
			self->velocity.x = 0;

			self->collider.size = (Vector2){ 0,0 };
		}
	}
		

	self->transform->position.x += self->velocity.x;
	self->transform->position.y += self->velocity.y * (keystate[SDL_SCANCODE_Z] ? 1.1f : 1.0f);


	if (self->health > 0)
	{
		GameObjectList list = GetInteractsExceptLayer(self, LAYER_GROUND | LAYER_ENEMY);


		GameObject* later_on_ground = is_on_platform(self, LAYER_GROUND, 16);

		if (list.Count > 0 || (!later_on_ground && GetInteracts(self).Count > 0))
		{
			self->transform->position.x -= self->velocity.x * player_speed;
		}

		if (!on_ground && later_on_ground)
		{
			//play_sound("resources/sounds/landing.wav");
		}
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
	if (self->health > 0) {

		//if (fabs(diff.x) > self->collider.size.x * 2 + 5) // 
		//{

		self->velocity.x = self->transform->left ? -1 : 1 * deltaTime * 100.0f;

		set_animator_state(self, "run", 0, false);

		//}
		//else
		//{
		//	self->velocity.x = 0;

		//	if (fabs(diff.y) <= self->collider.size.y)
		//	{
		//		set_animator_state(self, "attack", 0, false);

		//		float cur_time = get_time();

		//		if (cur_time >= self->attack_in_seconds_counter)
		//		{
		//			create_thread(attack, self);
		//			self->attack_in_seconds_counter = cur_time + self->attack_in_seconds;
		//		}
		//	}
		//	else
		//	{
		//		set_animator_state(self, "idle", 0, false);
		//	}

		//}

		self->transform->position.x += self->velocity.x;
		self->transform->position.y += self->velocity.y;

		GameObject* platform = is_on_platform(self, LAYER_GROUND, 1.0f);

		GameObjectList list = GetInteracts(self);

		//static GameObject* last_object = NULL;

		if (list.Count > 0)
		{
			for (int i = 0; i < list.Count; i++)
			{
				GameObject *go = list.List[i];

				if (go == NULL) continue;

				if (go == platform) continue;

				if (go == self->last_object) continue;

				if (go->layer == LAYER_PLAYER && go->velocity.y <= 0)
					go->health = 0;

				self->last_object = go;

				self->transform->left = !self->transform->left;

				break;
			}
		}
	}


	if (self->health <= 0 && !compare_animator_state(self, "die"))
	{
		destroy_after(self, 3.0f); // destroy after 3 seconds
		set_animator_state(self, "die", 0, false);
	}

}

void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/mario.png", true, create_vec2(16, 16)); // local_player creation
	{
		Point spawn_position = { 50,400 };
		Vector2 spawn_scale = { 2.0f, 2.0f };

		int animation_count = 5;

		Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

		strcpy(animations[0].state_name, "idle");
		{
			initialize_int_list(&animations[0].sprites);

			add_int_to_list(&animations[0].sprites, 0);
			//add_int_to_list(&animations[0].sprites, 1);
			//add_int_to_list(&animations[0].sprites, 2);
			//add_int_to_list(&animations[0].sprites, 3);

			animations[0].loop = true;

			animations[0].current_index = 0;
			animations[0].current_frame = 0;
			animations[0].wait_frame = 6;
		}

		strcpy(animations[1].state_name, "run");
		{
			initialize_int_list(&animations[1].sprites);

			add_int_to_list(&animations[1].sprites, 1);
			add_int_to_list(&animations[1].sprites, 2);
			add_int_to_list(&animations[1].sprites, 3);


			animations[1].loop = true;

			animations[1].current_index = 0;
			animations[1].current_frame = 0;
			animations[1].wait_frame = 6;
		}

		strcpy(animations[2].state_name, "jump");
		{
			initialize_int_list(&animations[2].sprites);

			add_int_to_list(&animations[2].sprites, 5);

			animations[2].loop = true;

			animations[2].current_index = 0;
			animations[2].current_frame = 0;
			animations[2].wait_frame = 6;
		}

		strcpy(animations[3].state_name, "fall-down");
		{
			initialize_int_list(&animations[3].sprites);

			add_int_to_list(&animations[3].sprites, 7);

			animations[3].loop = true;

			animations[3].current_index = 0;
			animations[3].current_frame = 0;
			animations[3].wait_frame = 6;
		}

		strcpy(animations[4].state_name, "die");
		{
			initialize_int_list(&animations[4].sprites);

			add_int_to_list(&animations[4].sprites, 6);
			//add_int_to_list(&animations[4].sprites, 10);
			//add_int_to_list(&animations[4].sprites, 11);
			//add_int_to_list(&animations[4].sprites, 12);

			animations[4].loop = false;

			animations[4].current_index = 0;
			animations[4].current_frame = 0;
			animations[4].wait_frame = 6;
		}

		GameObject* gameObject = GameObject_New(spawn_position, spawn_scale, (BoxCollider) { 0, 0, image->clip_size.x, image->clip_size.y }, LAYER_PLAYER, image, animations, animation_count, &localplayer_start, &localplayer_update); // spawning local player
	}

	image = LoadTexture("resources/environment/ground.png", false, (Vector2) { 0, 0 });

	GameObject_New(create_point(0, 500), (Vector2) { 1, 1 }, (BoxCollider) { 0, 20, image->rect.w, image->rect.h - 20 }, LAYER_GROUND, image, NULL, 0, 0, 0); // ground

	image = LoadTexture("resources/enemies/enemy3.png", true, (Vector2) { 32, 32 });
	{
		Point spawn_position = { 200,456 };
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
	for (int i = (int)pow(2, sizeof(LAYERS)-1); i != 1; i/=2)
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