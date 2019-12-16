#pragma once

//#ifndef GAMEOBJECT_H
//#define GAMEOBJECT_H

#include "Structs.h"

#include "Image.h"

//typedef struct GameObject;






Camera* camera;

Camera* create_camera(int screen_width, int screen_height);





Animation * get_animation_sprites(GameObject* self, char* state);


bool compare_animator_state(GameObject* self, char* state); // time is in seconds

bool set_animator_state(GameObject* self, char* state, float time, bool ignore_move); // time is in seconds



GameObject* GameObject_New(int id, Point position, Vector2 scale, BoxCollider collider, int layer, Image* image, Animation* animations, int animations_size, void* start, void* update);

void GameObject_Start(GameObject* self);

PointerList GameObject_BeforePhysics(GameObject* self);

GameObject* GameObject_Update(GameObject* self);

void GameObject_Physics(GameObject* self, PointerList before_interacts);

void GameObject_Draw(GameObject* self);




bool wait_to_make_process_on_gameobject_list;

void initialize_game_object_list(GameObjectList * list);

void add_game_object_to_list(GameObjectList * list, GameObject* game_object);

void delete_game_object_at(GameObjectList * list, int index);

int delete_game_object_from_list(GameObjectList * list, GameObject* game_object);


///

void initialize_animation_list(AnimationList * list);

void add_animation_to_list(AnimationList * list, Animation* game_object);



//#endif // !GAMEOBJECT_H
