#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"

#include "Utils.h"

#include "Collision.h"

#include "Audio.h"

#include <Windows.h> // for threads


GameObject* local_player = NULL;

float gravity = 20.0f;

float jump_force = 500.0f, player_speed = 5.0f, player_accel = 12.0f;


int wait_time = 0;

bool ignore_movement;

bool compare_animator_state(GameObject* self, char* state) // time is in seconds
{
	return strcmp(self->current_state, state) == 0;
}

bool set_animator_state(GameObject* self, char* state, float time, bool ignore_move) // time is in seconds
{
	if (compare_animator_state(self, state))
		return false;

	if (ignore_move) // logical
		ignore_movement = true;

	Uint32 current_time = SDL_GetTicks();

	if (wait_time > current_time) return false;

	wait_time = current_time + time * 1000.0f;

	Animation* animation = get_animation_sprites(self, state);

	if (animation)
		animation->current_index = 0;

	strcpy(self->current_state, state);

	return true;
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
		if (list.List[i] != NULL && LAYER_GROUND == list.List[i]->layer)
		{
			float foot_y = self->transform->position.y + self->image->clip_size.y * self->transform->scale.y;

			float ground_y = list.List[i]->transform->position.y + list.List[i]->collider.offset.y;

			if (float_compare(foot_y, ground_y, 16)) // eðer ayaðýn platformun üstündeyse
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
		if (list.List[i] == NULL)
			continue;

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

	self->velocity = (Vector2) { 0, 0 };

	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");

	self->health = 100;
}

Uint32 attack_cooldown = 700, attack_timer = 0;


void attack(GameObject* self)
{
	float timer = 0;

	while (timer <= 0.4f)
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
				go->health -= 5;

			to_break = true;

			printf("dmg to %X, left health: %d\n", go, go->health);
		}

		if (to_break)
			break;

		Sleep((int)(deltaTime * 1000));
	}
}


void localplayer_update(GameObject* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	GameObject* on_ground = is_on_platform(self);

	if (!on_ground)
	{
		if (self->velocity.y <= 15.0f)
			self->velocity.y += deltaTime * gravity;
		
			
		if(keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
			self->velocity.y += deltaTime * gravity;
	}
	else
	{
		self->velocity.y = 0;
	}
		
	if (self->health <= 0)
		set_animator_state(self, "die", 10, true);

	Uint32 current_time = SDL_GetTicks();

	if (ignore_movement && wait_time > current_time) return;

	ignore_movement = false;
	
	//if (keystate[SDL_SCANCODE_G])
	//{
	//	set_animator_state(self, "die", 5.0f, true);
	//}

	if ( keystate[SDL_SCANCODE_F] && attack_timer <= current_time )
	{
		set_animator_state(self, "attack", 0.525f, false);

		play_sound("resources/sounds/attack.wav");

		attack_timer = current_time + attack_cooldown;

		create_thread(attack, self);
		// TO DO, look for enemy and damage it
	}

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

		self->velocity.x += deltaTime * player_accel;
		self->velocity.x = clamp(self->velocity.x, -1, 1);

		set_animator_state(self, "run", 0, false);
	}
	else if(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])
	{
		self->transform->left = true;

		self->velocity.x -= deltaTime * player_accel;
		self->velocity.x = clamp(self->velocity.x, -1, 1);


		set_animator_state(self, "run", 0, false);
	}
	else
	{
		self->velocity.x -= self->velocity.x * deltaTime * player_accel;

		set_animator_state(self, "idle", 0, false);
	}

	Point camera_pos;
	camera_pos.x = self->transform->position.x + self->image->clip_size.x * 2 + self->velocity.x * 150.0f;
	camera_pos.y = self->transform->position.y - self->image->clip_size.y / 2 + self->velocity.y - 75;

	camera->position = point_lerp(camera->position, camera_pos, 0.12f); // deltaTime should be used? (deltaTime causes screen shakes)

	GameObjectList enemy_list;
	if ((enemy_list = GetInteractsOnlyLayer(self, LAYER_ENEMY)).Count > 0)
	{
		float mine_x_center = self->transform->position.x + self->collider.size.x / 2 + self->collider.offset.x;
		float enemy_x_center = enemy_list.List[0]->transform->position.x + enemy_list.List[0]->collider.size.x / 2 + enemy_list.List[0]->collider.offset.x;

		//float speed = clamp( mine_x_center - enemy_x_center , -1, 1);
		self->velocity.x += (enemy_list.List[0]->velocity.x) * deltaTime * 2.75f;
		//self->velocity.y = speed * deltaTime * -10.0f;
	}

	self->transform->position.x += self->velocity.x * player_speed;
	self->transform->position.y += self->velocity.y;

	GameObjectList list = GetInteractsExceptLayer(self, LAYER_GROUND | LAYER_ENEMY);

	GameObject* later_on_ground = is_on_platform(self);

	if (list.Count > 0 || (!later_on_ground && GetInteracts(self).Count > 0))
	{
		self->transform->position.x -= self->velocity.x * player_speed;
	}

	if (!on_ground && later_on_ground)
	{
		play_sound("resources/sounds/landing.wav");
	}

}

void test_start(GameObject* self) {

	self->health = 20;
	
	self->velocity = (Vector2) { 0, 0 };

}

void destroy_after(GameObject* object)
{
	Sleep(3 * 1000);

	delete_game_object_from_list(&GameObjects, object);
	
}

Uint32 test_enemy_attack_counter = 0;

void test_update(GameObject * self) {
	if (self->health > 0) {
		Vector2 mine_center = vec2_sum(self->transform->position, collider_center(self->collider));
		Vector2 enemy_center = vec2_sum(local_player->transform->position, collider_center(local_player->collider));

		Vector2 diff = vec2_minus(enemy_center, mine_center);

		if (fabs(diff.x) > self->collider.size.x * 2 + 5) // 
		{
			float clampd = clamp(diff.x, -1, 1);

			self->velocity.x = clampd * deltaTime * 100.0f;


			set_animator_state(self, "run", 0, false);

			if (diff.x < 0) {
				self->transform->left = true;
			}
			else
				self->transform->left = false;
		}
		else
		{
			self->velocity.x = 0;

			if (fabs(diff.y) <= self->collider.size.y)
			{
				set_animator_state(self, "attack", 0, false);

				Uint32 cur_time = SDL_GetTicks();

				if (cur_time >= test_enemy_attack_counter)
				{
					create_thread(attack, self);
					test_enemy_attack_counter = cur_time + 700;
				}
			}
			else
			{
				set_animator_state(self, "idle", 0, false);
			}

		}

		self->transform->position.x += self->velocity.x;
		self->transform->position.y += self->velocity.y;
	}


	if (self->health <= 0 && !compare_animator_state(self, "die"))
	{
		create_thread(destroy_after, self);
		set_animator_state(self, "die", 0, false);
	}

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