#include "Game.h"

#include "Globals.h"
#include "Draws.h"
#include "GameObject.h"
#include "Image.h"

#include "Utils.h"

#include "Collision.h"

#include "Audio.h"

#include "Time.h"

#include "Editor.h"

#include "Colors.h"

#include "Menu.h"

#include <Windows.h>



GameObject* local_player = NULL;

float gravity = 0.2f, max_y_speed = 4.0f;

float jump_force = 5.0f, player_speed = 200.0f, player_accel = 0.15f;

int max_jump_count = 2, jump_count = 2;

float on_ground_tolerance = 0.1f;

float enemy_distance_tolerance = 640;

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

	self->transform->position.y += self->velocity.y;
	
	GameObjectList list = GetInteractsExceptLayer(self, LAYER_EFFECTS | LAYER_TRAPS);
	
	for (int i = 0; i < list.Count; i++) {
	
		GameObject* go = list.List[i];
		
		if (go == NULL || go == self->owner)
			continue;

		if (go->layer == LAYER_ENEMY && go->health <= 0)
			continue;

		if (go->health > 0)
			go->health -= self->attack_force;

		play_sound("resources/sounds/boom.wav");

		GameObject* particle = create_particle("resources/effects/bullet-particle.png", (Vector2) { 12, 12 }, (Vector2) { 1, 1 }, point_sum((Point) { collider_center(self).x, collider_center(self).y }, vec2_multiply(self->velocity, deltaTime)), 4, 6);

		destroy_after(particle, .5f);

		return NULL; // means destroy self

		break;
	}

	free(list.List);

	return self;
}


void create_ammo(GameObject * self, char* texture, bool is_sprite_sheet, Vector2 sprite_size, Vector2 offset, Vector2 scale) {
	
	Image* image = LoadTexture(texture, is_sprite_sheet, sprite_size);
	
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

	Vector2 center = collider_center(self);

	Point ammoPosition = { center.x + (self->transform->left ? -offset.x : offset.x), center.y + offset.y };
	
	GameObject* ammo = GameObject_New(GameObjects.Count, ammoPosition, scale, (BoxCollider) { 0, 0, sprite_size.x, sprite_size.y }, LAYER_EFFECTS, image, animations, animation_count, &ammo_start, &ammo_update);
	
	ammo->owner = self;
	ammo->velocity.x = self->projectile_speed * (self->transform->left ? -1 : 1);
	ammo->attack_force = self->attack_force;
	ammo->transform->left = self->transform->left;

} 




void on_collision_enter(GameObject* self, GameObject* other)
{
	//printf("COLLISION ENTER!\n");

	if (other->layer == LAYER_ENEMY && !compare_animator_state(self, "dash") && other->health>0)
	{
		self->health -= 5;
		shake_camera(3.0f, 0.5f);

		self->extra_velocity.x = sign(self->transform->position.x - other->transform->position.x) * 2.0f;
	}
}

void on_collision_exit(GameObject* self, GameObject* other)
{
	//printf("COLLISION EXIT!\n");

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

	self->projectile_speed = player_speed;


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

#include "Pause.h"
void OnPlayerDead()
{
	curFrame = 1.0f;
	game_state = DIE;

	Sleep(5000);
	
	game_state = MENU;
}

GameObject* localplayer_update(GameObject* self)
{
	//DrawButtonInGame("button", (Rect) { 200, 350, 100, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, &onClick);
	//DrawButtonWithImageOnScreen("button", "resources/enemies/enemy.png", (Rect) { 200, 350, 100, 50 }, (Color) { 0, 0, 0, 255 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft, &onClick);

	if (game_state == END)
		return self;

	Vector2 healtbar_position = collider_center(self);
	healtbar_position.y -= self->collider.size.y*1.5;

	char buffer[12];
	snprintf(buffer, sizeof(buffer), "%d", self->health);

	DrawTextInGame(buffer, healtbar_position, (Color) { 234, 213, 142, 255 }, Font_Minecraft);

	
	if (self->transform->position.y > 3000)
		self->health = 0;

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

			set_animator_state(self, "jump", 0, 0, 0);
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

			if (keystate[SDL_SCANCODE_RIGHT] && last_input == SDL_SCANCODE_RIGHT)
			{
				self->transform->left = false;

				self->velocity.x = clamp(self->velocity.x + player_accel, -1.0f, 1.0f);

				set_animator_state(self, "run", 0, 0, 0);
			}
			else if (keystate[SDL_SCANCODE_LEFT] && last_input == SDL_SCANCODE_LEFT)
			{
				self->transform->left = true;

				self->velocity.x = clamp(self->velocity.x - player_accel, -1.0f, 1.0f);

				set_animator_state(self, "run", 0, 0, 0);
			}
			else // no input
			{
				self->velocity.x = float_lerp(self->velocity.x, 0, player_accel);
				if (on_ground)
					set_animator_state(self, "idle", 0, 0, 0);
				
			}
			if (!on_ground) {
				set_animator_state(self, "jump", 0, 0, 0);
			}

			if (keystate[SDL_SCANCODE_X] && self->attack_in_seconds_counter <= cur_time) // fire
			{
				play_sound("resources/sounds/shoot.wav");

				if (on_ground)
					set_animator_state(self, "shoot", self->attack_time, true, 0);
				else
					set_animator_state(self, "shootonair", self->attack_time, true, 0);


				create_ammo(self, "resources/effects/bullet.png", true, (Vector2) { 12, 12 }, (Vector2) { 50, -10 }, (Vector2) {1,1});

				self->attack_in_seconds_counter = cur_time + self->attack_in_seconds;

			}

			if (keystate[SDL_SCANCODE_Z] && self->dash_in_seconds_counter <= cur_time) // dash
			{
				set_animator_state(self, "dash", 0.35f, true, 0);

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

			if (interacted_object == NULL || interacted_object == later_on_ground || interacted_object->collider.is_trigger) continue;

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
		if (!(self->ignore_movement && self->ignore_movement_time > cur_time))
		{
			create_thread(OnPlayerDead, 0);

			set_animator_state(self, "die", 999, true, 0);
		}
	}

	return self;
}

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

void attack_enemy_one(GameObject* self)
{
	Sleep((int)(self->attack_preparation_time * 1000));

	if (self->health <= 0)
		return;

	play_sound("resources/sounds/enemy1_hit.wav");

	float timer = 0;

	while (timer <= self->attack_time)
	{
		if (self->health <= 0)
			break;

		timer += deltaTime;

		float x_size = (self->collider.size.x) * self->transform->scale.x;
		float y_size = (self->collider.size.x) * self->transform->scale.y;

		float forward = self->transform->left ? -1 : 1;

		BoxCollider boxcol;
		boxcol.size = (Vector2) { (x_size * 1.5f), y_size * 0.5f };
		boxcol.offset = (Vector2) { forward * (x_size + 4), y_size * 0.5f };

		bool to_break = false;

		Vector2 center = collider_center(self);
		Point point = { center.x, center.y };

		GameObjectList list = GetInteractsOfCollider(boxcol, point);
		for (int i = 0; i < list.Count; i++) {

			if (list.List[i] == NULL)
				continue;

			GameObject* go = list.List[i];

			if (go == self)
				continue;

			if (go->health > 0)
			{
				go->health -= self->attack_force;

				if (go->layer == LAYER_PLAYER)
					shake_camera(5.0f, 0.5f);
			}


			to_break = true;
		}

		free(list.List);

		if (to_break)
			break;

		Sleep((int)(deltaTime * 1000));
	}
}

void create_enemy_one(Vector2);


void on_enemy_collision_enter(GameObject* self, GameObject* other)
{
	if (other->layer == LAYER_GROUND)
	{
		self->grounded = true;
	}
}

void on_enemy_collision_exit(GameObject* self, GameObject* other)
{
	//if (other->layer == LAYER_GROUND)
	//{
	//	self->grounded = false;
	//}
}


void enemy_one_start(GameObject* self) {

	self->grounded = false;

	add_member_to_list(&self->collider.onEnter, on_enemy_collision_enter);
	add_member_to_list(&self->collider.onExit, on_enemy_collision_exit);

	self->health = 50;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 20;
	self->attack_range = 45;
	self->attack_in_seconds = 2.0f;
	self->attack_time = 0.2f;
	self->attack_preparation_time = 0.6f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

GameObject* enemy_one_update(GameObject * self) {


	if (!self->grounded)
		self->velocity.y += gravity * 18.0f;
	else
		self->velocity.y = 0;

	self->transform->position.y += self->velocity.y * deltaTime;


	float distance = fabs(collider_center(local_player).x - collider_center(self).x);

	if (distance > enemy_distance_tolerance)
		return self;

	if (self->health == INT_MIN)
		return self;
	
	char buffer[12];
	Vector2 healtbar = collider_center(self);
	healtbar.y -= self->collider.size.y * 1.5;

	snprintf(buffer, sizeof(buffer), "%d", self->health);
	DrawTextInGame(buffer, healtbar, (Color) { 234, 213, 142, 255 }, Font_Minecraft);
	
	if (self->health <= 0) {
	
		set_animator_state(self, "die", 999, 1, 1);
		self->health = INT_MIN;
		destroy_after(self, 3.0f);

		

		return self;
	}

	if (self->ignore_movement && self->ignore_movement_time >= get_time())
		return self;

	self->ignore_movement = false;

	float dist = fabs(collider_center(local_player).x- collider_center(self).x);

	float velx = sign(local_player->transform->position.x - self->transform->position.x);
	if (dist < self->attack_range) {

		if (local_player->health > 0 && self->attack_in_seconds_counter <= get_time()) {

			set_animator_state(self, "attack", self->attack_in_seconds, 1, 1);

			create_thread(&attack_enemy_one, self);
		}

	}
	else
	{
		set_animator_state(self, "run", 0, 0, 0);

		self->transform->position.x += velx * deltaTime * 150;
		
		self->transform->left = velx < 0;
	}

	
	return self;
}


void create_enemy_one(Vector2 position){

	Image* image = LoadTexture("resources/enemies/enemy.png", true, (Vector2) { 43 , 39 });
	int animation_count = 4;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 32; i <= 42; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "run");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 44; i <= 56; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "attack");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 17; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "die");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 18; i <= 32; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}



	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2, 2 }, (BoxCollider) { 8, 0, 27, 39 }, LAYER_ENEMY, image, animations, animation_count, &enemy_one_start, &enemy_one_update);
}

void enemy_two_start(GameObject* self) {

	self->grounded = false;

	add_member_to_list(&self->collider.onEnter, on_enemy_collision_enter);
	add_member_to_list(&self->collider.onExit, on_enemy_collision_exit);


	self->health = 10;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 10;
	self->attack_range = 45;
	self->attack_in_seconds = 2.0f;
	self->attack_time = 0.2f;
	self->attack_preparation_time = 0.6f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

GameObject* enemy_two_update(GameObject * self) {

	if (!self->grounded)
		self->velocity.y += gravity * 18.0f;
	else
		self->velocity.y = 0;

	self->transform->position.y += self->velocity.y * deltaTime;

	float distance = fabs(collider_center(local_player).x - collider_center(self).x);

	if (distance > enemy_distance_tolerance)
		return self;

	if (self->health == INT_MIN)
	{
		GameObjectList list = GetInteractsOnlyLayer(self, LAYER_GROUND);
		if (list.Count <= 0)
			self->transform->position.y += deltaTime * 100.0f;

		free(list.List);

		return self;
	}
		

	char buffer[12];
	Vector2 healtbar = collider_center(self);
	healtbar.y -= self->collider.size.y * 1.5;

	snprintf(buffer, sizeof(buffer), "%d", self->health);
	DrawTextInGame(buffer, healtbar, (Color) { 234, 213, 142, 255 }, Font_Minecraft);

	if (self->health <= 0) {

		set_animator_state(self, "die", 999, 1, 1);
		self->health = INT_MIN;
		destroy_after(self, 3.0f);

		

		return self;
	}

	if (self->ignore_movement && self->ignore_movement_time >= get_time())
		return self;

	self->ignore_movement = false;

	float distx = fabs(collider_center(local_player).x - collider_center(self).x);
	float disty = fabs(collider_center(local_player).y - collider_center(self).y);

	float velx = sign(local_player->transform->position.x - self->transform->position.x);
	float vely = sign(local_player->transform->position.y - self->transform->position.y);
	if (distx < self->attack_range && disty < self->attack_range) {

		if (local_player->health > 0 && self->attack_in_seconds_counter <= get_time()) {

			set_animator_state(self, "attack", 0.5, 1, 1);

			create_thread(&attack_enemy_one, self);
		}

	}
	else
	{
		
		set_animator_state(self, "idle", 0, 0, 0);
		self->transform->position.x += velx * deltaTime * 200;
		self->transform->position.y += vely * deltaTime * 50;
		self->transform->left = velx < 0;
	}




	return self;
}

void create_enemy_two(Vector2 position) {
	Image* image = LoadTexture("resources/enemies/enemytwo.png", true, (Vector2) { 16, 24 });
	int animation_count = 3;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 5; i <= 9; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	

	
	strcpy(animations[cur_anim].state_name, "die");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 10; i <= 14; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "attack");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 30; i <= 32; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}



	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2, 2 }, (BoxCollider) { 8, 0, 12,24  }, LAYER_ENEMY, image, animations, animation_count, &enemy_two_start, &enemy_two_update);
	}

void enemy_three_start(GameObject* self) {

	self->grounded = false;

	add_member_to_list(&self->collider.onEnter, on_enemy_collision_enter);
	add_member_to_list(&self->collider.onExit, on_enemy_collision_exit);

	self->health = 20;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 5;
	self->attack_range = 250;
	self->attack_in_seconds = 2.0f;
	self->attack_time = 0.2f;
	self->attack_preparation_time = 0.05f;

	self->projectile_speed = player_speed * 2.0f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

GameObject* enemy_three_update(GameObject * self) {

	if (!self->grounded)
		self->velocity.y += gravity * 18.0f;
	else
		self->velocity.y = 0;

	self->transform->position.y += self->velocity.y * deltaTime;

	float distance = fabs(collider_center(local_player).x - collider_center(self).x);

	if (distance > enemy_distance_tolerance)
		return self;

	if (self->health == INT_MIN)
		return self;

	char buffer[12];
	Vector2 healtbar = collider_center(self);
	healtbar.y -= self->collider.size.y * 1.5;

	snprintf(buffer, sizeof(buffer), "%d", self->health);
	DrawTextInGame(buffer, healtbar, (Color) { 234, 213, 142, 255 }, Font_Minecraft);

	if (self->health <= 0) {

		//set_animator_state(self, "die", 999, 1, 1);
		self->health = INT_MIN;
		//destroy_after(self, 3.0f);

		

		return NULL;
	}

	float velx = sign(local_player->transform->position.x - self->transform->position.x);

	self->transform->left = velx < 0;

	if (self->ignore_movement && self->ignore_movement_time >= get_time())
		return self;

	self->ignore_movement = false;

	float dist = fabs(collider_center(local_player).x - collider_center(self).x);



	if (dist < self->attack_range) {

		if (local_player->health > 0 && self->attack_in_seconds_counter <= get_time()) {

			set_animator_state(self, "attack", self->attack_in_seconds, 1, 1);

			play_sound("resources/sounds/shoot.wav");

			create_ammo(self, "resources/effects/arrow.png", true, (Vector2) { 16, 5 }, (Vector2) { 30, -15 }, (Vector2) {3,3});
		}

	}
	else
	{
		set_animator_state(self, "run", 0, 0, 0);

		self->transform->position.x += velx * deltaTime * 150;
	}




	return self;
}


void create_enemy_three(Vector2 position) {
	Image* image = LoadTexture("resources/enemies/enemy3.png", true, (Vector2) { 32, 32 });
	int animation_count = 3;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 0; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "run");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 3; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "attack");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 4; i <= 6; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}



	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 4.0f, 4.0f }, (BoxCollider) { 8, 0, 16, 21 }, LAYER_ENEMY, image, animations, animation_count, &enemy_three_start, &enemy_three_update);
}

void attack_boss(GameObject* self)
{
	Sleep((int)(self->attack_preparation_time * 1000));

	if (self->health <= 0)
		return;

	play_sound("resources/sounds/enemy1_hit.wav");

	float timer = 0;

	while (timer <= self->attack_time)
	{
		if (self->health <= 0)
			break;

		timer += deltaTime;

		Vector2 center = collider_center(self);
		Vector2 size = collider_size(self);

		float forward = self->transform->left ? -1 : 1;

		BoxCollider boxcol;

		boxcol.size = (Vector2) { (size.x * 0.5f), size.y * 0.5f };
		boxcol.offset = (Vector2) { forward * (size.x * 0.5f + 10), 0 };


		bool to_break = false;

		
		Point point = { center.x, center.y };

		GameObjectList list = GetInteractsOfCollider(boxcol, point);
		for (int i = 0; i < list.Count; i++) {

			if (list.List[i] == NULL)
				continue;

			GameObject* go = list.List[i];

			if (go == self)
				continue;

			if (go->health > 0)
			{
				go->health -= self->attack_force;

				if (go->layer == LAYER_PLAYER)
					shake_camera(5.0f, 0.5f);
			}


			to_break = true;
		}

		free(list.List);

		if (to_break)
			break;

		Sleep((int)(deltaTime * 1000));
	}
}

void boss_start(GameObject* self) {

	self->grounded = false;

	add_member_to_list(&self->collider.onEnter, on_enemy_collision_enter);
	add_member_to_list(&self->collider.onExit, on_enemy_collision_exit);


	self->health = 500;

	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 30;
	self->attack_range = 150;
	self->attack_in_seconds = 1.5f;
	self->attack_time = 0.2f;
	self->attack_preparation_time = 0.25f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

GameObject* boss_update(GameObject * self) {

	if (!self->grounded)
		self->velocity.y += gravity * 18.0f;
	else
		self->velocity.y = 0;

	self->transform->position.y += self->velocity.y * deltaTime;

	float distance = fabs(collider_center(local_player).x - collider_center(self).x);

	if (distance > enemy_distance_tolerance)
		return self;

	if (self->health == INT_MIN)
		return self;

	char buffer[12];
	Vector2 healtbar = collider_center(self);
	healtbar.y -= collider_size(self).y * 0.5f;

	snprintf(buffer, sizeof(buffer), "%d", self->health);
	DrawTextInGame(buffer, healtbar, (Color) { 234, 213, 142, 255 }, Font_Minecraft);

	if (self->health <= 0) {

		curFrame = 1.0f;
		game_state = END;

		set_animator_state(self, "die", 999, 1, 1);
		self->health = INT_MIN;
		destroy_after(self, 3.0f);

		

		return self;
	}

	if (self->ignore_movement && self->ignore_movement_time >= get_time())
		return self;

	self->ignore_movement = false;

	float dist = fabs(collider_center(local_player).x - collider_center(self).x);

	float velx = sign(local_player->transform->position.x - self->transform->position.x);
	if (dist < self->attack_range) {

		if (local_player->health > 0 && self->attack_in_seconds_counter <= get_time()) {

			int attack_type = rand()%2;
			
			if(attack_type==0)
			set_animator_state(self, "attack1", self->attack_in_seconds, 1, 1);
			
			if(attack_type==1)
			set_animator_state(self, "attack2", self->attack_in_seconds, 1, 1);



			create_thread(&attack_boss, self);
		}

	}
	else
	{
		set_animator_state(self, "run", 0, 0, 0);

		self->transform->position.x += velx * deltaTime * 150;

		self->transform->left = velx < 0;
	}




	return self;
}

void create_boss(Vector2 position) {
	Image* image = LoadTexture("resources/enemies/boss.png", true, (Vector2) { 96, 96 });
	int animation_count = 5;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 4; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "run");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 9; i <= 16; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "attack1");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 27; i <= 35; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}
	
	cur_anim++;

	strcpy(animations[cur_anim].state_name, "attack2");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 54; i <= 62; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;
	strcpy(animations[cur_anim].state_name, "die");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 81; i <= 86; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}



	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 4, 4 }, (BoxCollider) { 24, 24, 36,36  }, LAYER_ENEMY, image, animations, animation_count, &boss_start, &boss_update);

}

void trap_one_start(GameObject* self) {

	self->collider.is_trigger = true;


	self->velocity = (Vector2) { 0, 0 };

	self->attack_force = 50;
	self->attack_range = 150;
	self->attack_in_seconds = 1.5f;
	self->attack_time = 0.2f;
	self->attack_preparation_time = 0.25f;


	self->attack_in_seconds_counter = 0;
	self->ignore_movement_time = 0;
	self->ignore_movement = false;
}

void timer(GameObject * self) {
	float time = 0.3f;
	Sleep(500);

	while (time > 0) {
		GameObjectList list=  GetInteractsOnlyLayer(self, LAYER_PLAYER);
		for (int i = 0; i < list.Count; i++) {

			if (list.List[i]->health > 0)
			{
				list.List[i]->health -= self->attack_force;
				list.List[i]->extra_velocity.y = -3.0f;
			}
			
		}
		if (list.Count > 0) {
			break;
		}
		time -= deltaTime;
		Sleep(deltaTime * 1000);
		
	}

}

GameObject* trap_one_update(GameObject * self) {

	if (self->health == INT_MIN)
		return self;

	if (self->ignore_movement && self->ignore_movement_time >= get_time())
		return self;

	self->ignore_movement = false;

	set_animator_state(self, "attack", 2, 1, 1);

	create_thread(timer, self);

	return self;
}

void create_trap_one(Vector2 position) {
	Image* image = LoadTexture("resources/enemies/trapone.png", true, (Vector2) { 32, 32 });
	int animation_count = 2;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 0; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;
	

	
	strcpy(animations[cur_anim].state_name, "attack");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 1; i <= 13; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;

	



	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2, 2 }, (BoxCollider) { 0, 0, 32, 32 }, LAYER_TRAPS, image, animations, animation_count, &trap_one_start, &trap_one_update);
}

void create_trap_two(Vector2 position) {
	Image* image = LoadTexture("resources/enemies/traptwo.png", true, (Vector2) { 32, 41 });
	int animation_count = 2;

	Animation* animations = (Animation*)malloc(sizeof(Animation) * animation_count);

	int cur_anim = 0;

	strcpy(animations[cur_anim].state_name, "idle");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 0; i <= 0; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);

		animations[cur_anim].loop = true;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 6;
	}

	cur_anim++;



	strcpy(animations[cur_anim].state_name, "attack");
	{
		initialize_int_list(&animations[cur_anim].sprites);

		for (int i = 1; i <= 13; i++)
			add_int_to_list(&animations[cur_anim].sprites, i);


		animations[cur_anim].loop = false;

		animations[cur_anim].current_index = 0;
		animations[cur_anim].current_frame = 0;
		animations[cur_anim].wait_frame = 4;
	}

	cur_anim++;

	Point enemypos = *(Point*)&position;

	GameObject* enemy = GameObject_New(GameObjects.Count, enemypos, (Vector2) { 2,2 }, (BoxCollider) { 0, 0, 32, 41 }, LAYER_TRAPS, image, animations, animation_count, &trap_one_start, &trap_one_update);
}

void create_player(Vector2 position) {
	Image* image = LoadTexture("resources/players/player.png", true, create_vec2(48, 48)); // local_player creation
	{
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


			animations[3].loop = false;

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

		Point spawn_position = *(Point*)&position;

		GameObject* gameObject = GameObject_New(GameObjects.Count, spawn_position, spawn_scale, (BoxCollider) { 12, 4, 24, 35 }, LAYER_PLAYER, image, animations, animation_count, & localplayer_start, & localplayer_update); // spawning local player

	}
}

void _create_ground(Vector2 position) {
	Image* image = LoadTexture("resources/environment/stone_ground.png", false, create_vec2(417,417));

	Point ground_position = *(Point*)&position;

	GameObject* gameobject = GameObject_New(GameObjects.Count, ground_position, create_vec2(0.2f, 0.2f), (BoxCollider) {-5, 0, image->rect.w + 10, image->rect.h}, LAYER_GROUND, image, NULL, 0, 0, 0);
}

void create_ground_corner_left(Vector2 position) {
	Image* image = LoadTexture("resources/environment/stone_ground_corner_left.png", false, create_vec2(417,417));

	Point ground_position = *(Point*)&position;

	GameObject* gameobject = GameObject_New(GameObjects.Count, ground_position, create_vec2(0.2f, 0.2f), (BoxCollider) { -5, 0, image->rect.w + 10, image->rect.h}, LAYER_GROUND, image, NULL, 0, 0, 0);
}

void create_ground_corner_right(Vector2 position) {
	Image* image = LoadTexture("resources/environment/stone_ground_corner_right.png", false, create_vec2(417,417));

	Point ground_position = *(Point*)&position;

	GameObject* gameobject = GameObject_New(GameObjects.Count, ground_position, create_vec2(0.2f, 0.2f), (BoxCollider) { -5, 0, image->rect.w + 10, image->rect.h }, LAYER_GROUND, image, NULL, 0, 0, 0);

}

void create_ground_right(Vector2 position) {
	Image* image = LoadTexture("resources/environment/stone_ground_right.png", false, create_vec2(417, 417));

	Point ground_position = *(Point*)&position;

	GameObject* gameobject = GameObject_New(GameObjects.Count, ground_position, create_vec2(0.2f, 0.2f), (BoxCollider) { -5, 0, image->rect.w + 10, image->rect.h }, LAYER_GROUND, image, NULL, 0, 0, 0);
}

void create_ground_left(Vector2 position) {
	Image* image = LoadTexture("resources/environment/stone_ground_left.png", false, create_vec2(417, 417));

	Point ground_position = *(Point*)&position;

	GameObject* gameobject = GameObject_New(GameObjects.Count, ground_position, create_vec2(0.2f, 0.2f), (BoxCollider) { -5, 0, image->rect.w + 10, image->rect.h }, LAYER_GROUND, image, NULL, 0, 0, 0);
}


void Start()
{
	//if (ToBeDestroyed.List != NULL)
	//{
	//	for (int i = 0; i < ToBeDestroyed.Count; i++)
	//		free(ToBeDestroyed.List[i]);

	//	free(ToBeDestroyed.List);
	//}

	//for (int i = 0; i < GameObjects.Count; i++)
	//{
	//	destroy_object(GameObjects.List[i], 1);
	//	delete_game_object_at(&GameObjects, 0);
	//}

	initialize_game_object_list(&GameObjects);
	initialize_list(&ToBeDestroyed);

	char load_location[512];
	strcpy(load_location, "resources\\map\\");
	strcat(load_location, MapInputBox.buffer);
	strcat(load_location, ".png");

	InitializeMap(load_location);
	
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
	render_background();

	// UI things
	char fps_str[16];
	snprintf(fps_str, sizeof(fps_str), "%d fps", (int)(1.0f / deltaTime));

	DrawTextOnScreen(fps_str, (Vector2) { 50, 20 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);
	DrawTextInGame("litt-game", (Vector2) { 100, 400 }, (Color) { 255, 255, 255, 255 }, Font_Minecraft);

	////////////


	Uint8* keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_ESCAPE] && game_state != DIE && game_state != END)
	{
		game_state = PAUSE;
	}

	////////////

	
	for (int i = (int)pow(2, 5); i != 1; i /= 2)
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

				free(interact_list.List);

				GameObject_Draw(gameObject);
			}
		}

		free(list.List);


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

	case 1:
		create_enemy_two(position);
		break;

	case 2:
		create_enemy_three(position);
		break;
	
	case 3:
		create_boss(position);
		break;
	}
	
}

void create_trap(int index, Vector2 position)
{
	switch (index)
	{
	case 0:
		create_trap_one(position);
		break;

	case 1:
		create_trap_two(position);
		break;

	}

}

void create_ground(int index, Vector2 position) {
	switch (index) {
	case 0:
		_create_ground(position);
		break;
	case 1: 
		create_ground_corner_left(position);
		break;
	case 2:
		create_ground_corner_right(position);
		break;
	case 3:
		create_ground_left(position);
		break;
	case 4:
		create_ground_right(position);
		break;
	}
}