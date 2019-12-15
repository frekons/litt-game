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

float gravity = 0.2f, max_y_speed = 4.0f;

float jump_force = 6.0f, player_speed = 200.0f, player_accel = 0.15f;

float on_ground_tolerance = 0.1f;

void create_particle(GameObject* self){
	Image* image = LoadTexture("resources/effects/particle.png", true, (Vector2) { 12, 12 });
	int animation_count = 1;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	strcpy(animations[0].state_name, "idle");
	{
		initialize_int_list(&animations[0].sprites);

		add_int_to_list(&animations[0].sprites, 0);
		add_int_to_list(&animations[0].sprites, 1);
		add_int_to_list(&animations[0].sprites, 2);
		add_int_to_list(&animations[0].sprites, 3);


		animations[0].loop = false;

		animations[0].current_index = 0;
		animations[0].current_frame = 0;
		animations[0].wait_frame = 6;
	}
	Point ammopos;
	ammopos.x = self->transform->position.x+(self->velocity.x*deltaTime);
	ammopos.y = self->transform->position.y;

	GameObject* ammo = GameObject_New(GameObjects.Count, ammopos, (Vector2) { 2, 2 }, (BoxCollider) { 0, 0, 0 , 0}, LAYER_EFFECTS, image, animations, animation_count, 0, 0);
	destroy_after(ammo, 0.5f);

}

void ammo_start(GameObject* self) {

	destroy_after(self, 10);
	
}

GameObject* ammo_update(GameObject* self) {
	
	self->transform->position.x += self->velocity.x*deltaTime;
	
	GameObjectList list = GetInteracts(self);
	
	for (int i = 0; i < list.Count; i++) {
	
		GameObject* go = list.List[i];
		
		if (go == NULL || go == self->owner)
			continue;
		
		if (go->health > 0)
			go->health -= self->attack_force;

		create_particle(self);

		destroy_after(self, 0);

		break;
	}
	return self;
}


void create_ammo(GameObject * self) {
	Image* image = LoadTexture("resources/effects/bullet.png", true, (Vector2) {12,12});
	int animation_count = 1;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	strcpy(animations[0].state_name, "idle");
	{
		initialize_int_list(&animations[0].sprites);

		add_int_to_list(&animations[0].sprites, 0);
		add_int_to_list(&animations[0].sprites, 1);
		

		animations[0].loop = true;

		animations[0].current_index = 0;
		animations[0].current_frame = 0;
		animations[0].wait_frame = 6;
	}
	Point ammopos;
	ammopos.x = self->transform->position.x + (self->transform->left ? -5 : 89);
	ammopos.y = self->transform->position.y + 30;
	
	GameObject* ammo = GameObject_New(GameObjects.Count, ammopos, (Vector2) { 1, 1 }, (BoxCollider) { 0, 0, 12, 12 }, LAYER_EFFECTS, image, animations, animation_count, &ammo_start, &ammo_update);
	ammo->owner = self;
	ammo->velocity.x = 200 * (self->transform->left ? -1 : 1);
	ammo->attack_force = self->attack_force;

} 


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

GameObject* localplayer_update(GameObject* self)
{
	DrawTextInGame("test", (Vector2) { 100, 400 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);

	float cur_time = get_time();

	if (self->health > 0) // alive
	{
		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		camera_position(self);

		GameObject* on_ground = is_on_platform(self, LAYER_GROUND|LAYER_EFFECTS, on_ground_tolerance);

		if (on_ground)
		{
			self->velocity.y = float_lerp(self->velocity.y, 0, deltaTime * 5.0f);

			if (keystate[SDL_SCANCODE_UP]) {
				self->velocity.y = -jump_force;
				set_animator_state(self, "jump", 0, 0);
			}
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
				if (on_ground)
					set_animator_state(self, "shoot", 0.45f, true);
				else
					set_animator_state(self, "shootonair", 0.45f, true);


				create_ammo(self);

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
				if (on_ground)
					set_animator_state(self, "idle", 0, 0);
				
			}
			if (!on_ground) {
				set_animator_state(self, "jump", 0, 0);
			}

			if (keystate[SDL_SCANCODE_Z] && self->dash_in_seconds_counter <= cur_time) // dash
			{
				set_animator_state(self, "dash", 0.35f, true);

				self->extra_velocity.x = ( self->transform->left ? -1.0f : 1.0f ) * self->dash_force;

				self->dash_in_seconds_counter = cur_time + self->dash_in_seconds;
			}
		}
		else // no input
		{
			self->velocity.x = float_lerp(self->velocity.x, 0, player_accel);
			
		}


		Point old_position = self->transform->position;
		self->transform->position = point_sum(self->transform->position, vec2_multiplier(vec2_sum((Point) { self->velocity.x, self->velocity.y }, self->extra_velocity), deltaTime * player_speed));

		//
		GameObjectList interact_list = GetInteractsExceptLayer(self,LAYER_ENEMY);

		GameObject* later_on_ground = is_on_platform(self, LAYER_GROUND|LAYER_EFFECTS, on_ground_tolerance);

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
	return self;
}

void enemy_start(GameObject* self) {

	self->health = 50;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 20;
	self->attack_in_seconds = 0.7f;
	self->attack_time = 0.4f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

void create_enemy(void);

float angle = 0;

GameObject* enemy_update(GameObject * self) {
	
	
	float value = cosf((angle += deltaTime * 10) * 3.14159f / 180.0f) * 200;
	self->transform->position.x = 200+sinf((angle += deltaTime * 10) * 3.14159f / 180.0f) * 200;
	if (value < 0)
		self->transform->left = true;
	else
		self->transform->left = false;

	if (angle >= 360)
		angle = 0;


	if (self->health <= 0) {
		angle = 0;
		create_enemy();
		destroy_object(self);
		return NULL;
	}
	return self;
}


void create_enemy(void){
	Image* image = LoadTexture("resources/enemies/enemy.png", true, (Vector2) { 64 , 64 });
	int animation_count = 1;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	strcpy(animations[0].state_name, "idle");
	{
		initialize_int_list(&animations[0].sprites);

		add_int_to_list(&animations[0].sprites, 8);
		add_int_to_list(&animations[0].sprites, 9);
		add_int_to_list(&animations[0].sprites, 10);
		add_int_to_list(&animations[0].sprites, 11);


		animations[0].loop = true;

		animations[0].current_index = 0;
		animations[0].current_frame = 0;
		animations[0].wait_frame = 6;
	}
	Point enemypos;
	enemypos.x = rand() % 100;
	enemypos.y = 440;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2, 2 }, (BoxCollider) { 0, 0, 36, 64 }, LAYER_ENEMY, image, animations, animation_count,&enemy_start,&enemy_update);
	
	
	

}


void Start()
{
	initialize_game_object_list(&GameObjects);
	//

	Image* image = LoadTexture("resources/players/player.png", true, create_vec2(48, 48)); // local_player creation
	{
		Point spawn_position = { 50,400 };
		Vector2 spawn_scale = { 2.0f, 2.0f };

		int animation_count = 7;

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
		
		
		strcpy(animations[4].state_name, "jump");
		{
			initialize_int_list(&animations[4].sprites);

			add_int_to_list(&animations[4].sprites, 33);
			


			animations[4].loop = true;

			animations[4].current_index = 0;
			animations[4].current_frame = 0;
			animations[4].wait_frame = 6;
		}

		strcpy(animations[5].state_name, "shootonair");
		{
			initialize_int_list(&animations[5].sprites);

			add_int_to_list(&animations[5].sprites, 40);
			add_int_to_list(&animations[5].sprites, 41);
			add_int_to_list(&animations[5].sprites, 42);
			add_int_to_list(&animations[5].sprites, 43);
			add_int_to_list(&animations[5].sprites, 44);

			animations[5].loop = false;

			animations[5].current_index = 0;
			animations[5].current_frame = 0;
			animations[5].wait_frame = 6;
		}
		strcpy(animations[6].state_name, "dash");
		{
			initialize_int_list(&animations[6].sprites);

			add_int_to_list(&animations[6].sprites, 34);



			animations[6].loop = false;

			animations[6].current_index = 0;
			animations[6].current_frame = 0;
			animations[6].wait_frame = 6;
		}

		

		GameObject* gameObject = GameObject_New(GameObjects.Count, spawn_position, spawn_scale, (BoxCollider) { 12, 4, 24, 35 }, LAYER_PLAYER, image, animations, animation_count, &localplayer_start, &localplayer_update); // spawning local player
	}

	image = LoadTexture("resources/environment/ground.png", false, (Vector2) { 0, 0 });

	GameObject_New(GameObjects.Count, create_point(0, 500), (Vector2) { 1, 1 }, (BoxCollider) { 0, 40, image->rect.w, image->rect.h - 40 }, LAYER_GROUND, image, NULL, 0, 0, 0); // ground

	create_enemy();
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
	
	for (int i = (int)pow(2, 4); i != 1; i /= 2)
	{
		GameObjectList list = GetObjectsOfLayer(i);

		for (int j = 0; j < list.Count; j++)
		{
			GameObject* gameObject = list.List[j];

			if (gameObject != NULL)
			{
				gameObject = GameObject_Update(gameObject);

				GameObject_Draw(gameObject);
			}
		}


	}

}