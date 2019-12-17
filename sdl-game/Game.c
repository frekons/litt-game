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

float jump_force = 5.0f, player_speed = 200.0f, player_accel = 0.15f;

int max_jump_count = 2, jump_count = 2;

float on_ground_tolerance = 0.1f;

GameObject* create_particle(char* image_directory, Vector2 clip_size, Vector2 scale, Point position, int sprite_count, int wait_frame) 
{
	Image* image = LoadTexture(image_directory, true, clip_size);

	Animation* animations = (Animation*)malloc(sizeof(Animation) * 1); // will only accept 1 animation

	strcpy(animations[0].state_name, "idle");
	{
		initialize_int_list(&animations[0].sprites);

		for (int i = 0; i < sprite_count; i++)
		{
			add_int_to_list(&animations[0].sprites, i);
		}


		animations[0].loop = false;

		animations[0].current_index = 0;
		animations[0].current_frame = 0;
		animations[0].wait_frame = wait_frame;
	}

	GameObject* ammo_particle = GameObject_New(GameObjects.Count, position, scale, (BoxCollider) { 0, 0, 0, 0 }, LAYER_EFFECTS, image, animations, 1, 0, 0);

	return ammo_particle;
}

void ammo_start(GameObject* self) {

	destroy_after(self, 10);
	
}

GameObject* ammo_update(GameObject* self) {
	
	if (self == NULL)
		return self;

	self->transform->position.x += self->velocity.x * deltaTime;
	
	GameObjectList list = GetInteracts(self);
	
	for (int i = 0; i < list.Count; i++) {
	
		GameObject* go = list.List[i];
		
		if (go == NULL || go == self->owner)
			continue;
		
		if (go->health > 0)
			go->health -= self->attack_force;

		play_sound("resources/sounds/boom.wav");

		GameObject* particle = create_particle("resources/effects/bullet-particle.png", (Vector2) { 12, 12 }, (Vector2) {1,1}, point_sum(self->transform->position, vec2_multiply(self->velocity, deltaTime)), 4, 6);

		destroy_after(particle, .5f);

		return NULL; // means destroy self

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
	ammopos.y = self->transform->position.y + 35;
	
	GameObject* ammo = GameObject_New(GameObjects.Count, ammopos, (Vector2) { 1, 1 }, (BoxCollider) { 0, 0, 12, 12 }, LAYER_EFFECTS, image, animations, animation_count, &ammo_start, &ammo_update);
	ammo->owner = self;
	ammo->velocity.x = player_speed * (self->transform->left ? -1 : 1);
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

void on_collision_enter(GameObject* self, GameObject* other)
{
	printf("COLLISION ENTER!\n");

}

void on_collision_exit(GameObject* self, GameObject* other)
{
	printf("COLLISION EXIT!\n");

}

void localplayer_start(GameObject* self)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_INFO, "Local Player has spawned!");

	local_player = self;

	add_member_to_list(&self->collider.onEnter, &on_collision_enter);
	add_member_to_list(&self->collider.onExit, &on_collision_exit);

	self->health = 100;

	self->attack_force = 10;
	self->attack_in_seconds = 0.35f;
	self->attack_time = 0.2f;

	self->dash_in_seconds = 1.0f;
	self->dash_force = 3.0f;
}

void camera_position(GameObject* object)
{
	Point position;
	position.x = object->transform->position.x + object->image->clip_size.x;
	position.y = object->transform->position.y - object->image->clip_size.y / 2;

	camera->position = position;
}

void onClick(void)
{
	local_player->velocity.y = -jump_force;
}


GameObject* localplayer_update(GameObject* self)
{
	//DrawButtonInGame("button", (Rect) { 200, 350, 100, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, &onClick);
	//DrawButtonWithImageOnScreen("button", "resources/enemies/enemy.png", (Rect) { 200, 350, 100, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, &onClick);


	float cur_time = get_time();

	if (self->health > 0) // alive
	{
		Uint8* keystate = SDL_GetKeyboardState(NULL);

		camera_position(self);

		GameObject* on_ground = is_on_platform(self, LAYER_GROUND|LAYER_EFFECTS, on_ground_tolerance);

		if (on_ground)
		{
			self->velocity.y = float_lerp(self->velocity.y, 0, deltaTime * 5.0f);

			jump_count = max_jump_count;
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

		if (jump_count > 0 && keystate[SDL_SCANCODE_UP]) {

			self->velocity.y = -jump_force;

			set_animator_state(self, "jump", 0, 0);
			play_sound("resources/sounds/jump.wav");

			Vector2 center = collider_center(self);
			Vector2 down = collider_left_down(self);
			Point position = { center.x - 64, down.y - 64 };
			
			
			destroy_after(create_particle("resources/effects/smoke.png", (Vector2) { 256, 128 }, (Vector2) {0.5f, 0.5f}, position, 6, 4), 0.45f);

			jump_count--;

			keystate[SDL_SCANCODE_UP] = 0;
		}

		if (!(self->ignore_movement && self->ignore_movement_time > cur_time))
		{

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

			if (keystate[SDL_SCANCODE_X] && self->attack_in_seconds_counter <= cur_time) // fire
			{
				play_sound("resources/sounds/shoot.wav");

				if (on_ground)
					set_animator_state(self, "shoot", self->attack_time, true);
				else
					set_animator_state(self, "shootonair", self->attack_time, true);


				create_ammo(self);

				self->attack_in_seconds_counter = cur_time + self->attack_in_seconds;

			}

			if (keystate[SDL_SCANCODE_Z] && self->dash_in_seconds_counter <= cur_time) // dash
			{
				set_animator_state(self, "dash", 0.35f, true);

				play_sound("resources/sounds/dash.wav");

				self->extra_velocity.x = ( self->transform->left ? -1.0f : 1.0f ) * self->dash_force;

				self->dash_in_seconds_counter = cur_time + self->dash_in_seconds;
			}
		}
		else // no input
		{
			self->velocity.x = float_lerp(self->velocity.x, 0, player_accel);
			
		}

		self->animations.List[1]->wait_frame = 5 / fabs(self->velocity.x); // run animation speed

		Point old_position = self->transform->position;
		self->transform->position = point_sum(self->transform->position, vec2_multiply(vec2_sum((Point) { self->velocity.x, self->velocity.y }, self->extra_velocity), deltaTime * player_speed));

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

void create_enemy_one(void);

float angle = 0;

float lineer_sin(float val, int* slope)
{
	if (val <= 2)
	{
		if (slope)
			*slope = 1;

		return val - 1;
	}
	else if (val <= 4)
	{
		if (slope)
			*slope = -1;

		return -val + 3;
	}

	return 0;
}

GameObject* enemy_update(GameObject * self) {
	
	
	int slope;
	self->transform->position.x = 200 + lineer_sin(angle, &slope) * 200;

	if (slope < 0)
		self->transform->left = true;
	else
		self->transform->left = false;

	angle += deltaTime * 0.75f;

	if (angle > 4)
		angle = 0;


	if (self->health <= 0) {
		angle = 0;
		create_enemy_one((Vector2) { 200, 440 });
		
		return NULL; // means destroy self
	}

	return self;
}


void create_enemy_one(Vector2 position){
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
	enemypos.x = position.x;
	enemypos.y = position.y;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2, 2 }, (BoxCollider) { 0, 0, 36, 64 }, LAYER_ENEMY, image, animations, animation_count,&enemy_start,&enemy_update);
	
	
	

}


void Start()
{
	
	initialize_game_object_list(&GameObjects);
	initialize_list(&ToBeDestroyed);
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

	create_enemy_one((Vector2) { 200, 440 });
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

bool wait_to_process_on_to_be_destroyed = false;


void Render()
{
	// UI things
	char fps_str[16];
	snprintf(fps_str, sizeof(fps_str), "%d fps", (int)(1.0f / deltaTime));

	DrawTextOnScreen(fps_str, (Vector2) { 50, 20 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
	DrawTextInGame("litt-game", (Vector2) { 100, 400 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);

	////////////
	
	for (int i = (int)pow(2, 4); i != 1; i /= 2)
	{
		GameObjectList list = GetObjectsOfLayer(i);

		for (int j = 0; j < list.Count; j++)
		{
			GameObject* gameObject = list.List[j], *temp = gameObject;

			if (gameObject != NULL)
			{
				PointerList interact_list = GameObject_BeforePhysics(gameObject);

				gameObject = GameObject_Update(gameObject);

				if (gameObject == NULL)
				{
					destroy_object(temp, true);
					continue;
				}

				GameObject_Physics(gameObject, interact_list);

				GameObject_Draw(gameObject);
			}
		}


	}


	wait_to_process_on_to_be_destroyed = true;
	for (int i = 0; i < ToBeDestroyed.Count; i++)
	{
		DestroyInfo* to_destroy = ToBeDestroyed.List[i];

		to_destroy->time -= deltaTime;

		if (to_destroy->time <= 0)
		{
			destroy_object(to_destroy->object, false);

			to_destroy->destroyed = true;
		}
	}

	for (int i = 0; i < ToBeDestroyed.Count; i++)
	{
		DestroyInfo* to_destroy = ToBeDestroyed.List[i];

		if (to_destroy->destroyed)
		{
			free(to_destroy);

			delete_member_at(&ToBeDestroyed, i--);
		}
	}

	wait_to_process_on_to_be_destroyed = false;

}



void create_enemy(int index, Vector2 position)
{
	switch (index)
	{
	case 0:
		create_enemy_one(position);
		break;

	}

}