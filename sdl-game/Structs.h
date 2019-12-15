#pragma once

//#ifndef STRUCTS_H
//#define STRUCTS_H

#include <SDL2/SDL.h>
#include <stdbool.h>


struct SIntList {

	int Count;
	int* List;

};

typedef struct SIntList IntList;

void initialize_int_list(IntList * list);

void add_int_to_list(IntList * list, int member);


typedef struct SVector2 {
	float x, y;
}Vector2;

Vector2 create_vec2(float x, float y);

struct SSize {
	int w, h;
};

typedef struct SSize Size;

typedef struct SDL_Rect Rect;

typedef struct SDL_FPoint Point;

Point create_point(float x, float y);


Rect create_rect(int x, int y, int w, int h);




struct SColor {
	Uint8 r, g, b, a;
};

typedef struct SColor Color;

Color create_color(int r, int g, int b, int a);


typedef struct SImage {
	SDL_Texture* texture;
	Rect rect;

	bool sprite_sheet;
	Vector2 clip_size;
}Image;


struct SBoxCollider
{
	Vector2 offset, size;
};

typedef struct SBoxCollider BoxCollider;


struct STransform {
	Point position;
	Vector2 scale;
	bool left;
	//GameObject* gameObject;
};

typedef struct STransform Transform;

typedef struct Animation {
	char state_name[32];
	IntList sprites;

	bool loop;

	int current_index;
	int wait_frame, current_frame;
}Animation;

struct SAnimationList {

	int Count;
	Animation** List;

};
typedef struct SAnimationList AnimationList;


typedef struct SCamera {

	Point position;
	int width, height;

}Camera;

typedef struct SGameObject {

	int id;

	Transform* transform;

	BoxCollider collider;

	Image *image;

	AnimationList animations;

	char current_state[32];

	void* start;

	void* update;

	int layer;
	
	int health;

	Vector2 velocity, extra_velocity;

	bool ignore_movement;
	float ignore_movement_time;

	int attack_force;
	float attack_in_seconds, attack_in_seconds_counter, attack_time; 

	float dash_in_seconds, dash_in_seconds_counter, dash_force;

	struct GameObject* last_object,*owner;
}GameObject;


struct SGameObjectList {

	int Count;
	GameObject** List;

};


typedef struct SGameObjectList GameObjectList;


typedef enum{ // upper means top in rendering

	LAYERS_NONE = 0, // not rendering this
	LAYERS_PLAYER,
	LAYERS_ENEMY,
	LAYERS_GROUND,
	LAYERS_EFFECTS,

}LAYERS;

//#define LAYER_NONE 1 << LAYERS_NONE /* not rendering */
#define LAYER_GROUND 1 << LAYERS_GROUND
#define LAYER_PLAYER 1 << LAYERS_PLAYER
#define LAYER_ENEMY 1 << LAYERS_ENEMY
#define LAYER_EFFECTS 1 << LAYERS_EFFECTS


struct SPointerList {

	int Count;
	void** List;

};


typedef struct SPointerList PointerList;



void initialize_list(PointerList * list);

void add_member_to_list(PointerList * list, void* texture);

void delete_member_at(PointerList * list, int index);

void delete_member_from_list(PointerList * list, void* member);


//#endif