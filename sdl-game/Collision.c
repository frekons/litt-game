
#include "Game.h"

#include "Draws.h"

#include "Globals.h"

#include "Utils.h"

//#define DEBUG_COLLISION

bool IsInteractingRect(Rect col, Vector2Int point) 
{
	// offset is used as position

	return point.x <= col.x + col.w &&
		point.x >= col.x &&
		point.y <= col.y + col.h &&
		point.y >= col.y;

}

bool IsInteractingPoint(BoxCollider col, Point point) 
{
	// offset is used as position

	if ((col.size.x == 0 && col.size.y == 0))
		return false;

	return point.x <= col.offset.x + col.size.x &&
		point.x >= col.offset.x &&
		point.y <= col.offset.y + col.size.y &&
		point.y >= col.offset.y;

}


bool IsInteracting(BoxCollider col1, BoxCollider col2) {
	// offset is used as position

	if ((col1.size.x == 0 && col1.size.y == 0) || (col2.size.x == 0 && col2.size.y == 0))
		return false;

	return col1.offset.x <= col2.offset.x + col2.size.x &&
		col1.offset.x + col1.size.x >= col2.offset.x &&
		col1.offset.y <= col2.offset.y + col2.size.y &&
		col1.offset.y + col1.size.y >= col2.offset.y;

}



GameObjectList GetInteractsOfCollider(BoxCollider col1, Point position)
{
	GameObjectList list;
	initialize_game_object_list(&list);

	col1.offset.x += position.x;
	col1.offset.y += position.y;

#ifdef DEBUG_COLLISION
	if (renderable_state)
		DrawRectangleOnScreen((Rect) {
		col1.offset.x - (camera->position.x - camera->width / 2), col1.offset.y - (camera->position.y - camera->height / 2),
			col1.size.x, col1.size.y
	}, (Color) { 255, 0, 0, 255 });
#endif

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* go = GameObjects.List[i];
		if (go == NULL)
			continue;
		BoxCollider col2 = go->collider;
		col2.offset.x += go->transform->position.x;
		col2.offset.y += go->transform->position.y;

		if (col1.offset.x == col2.offset.x && col1.offset.y == col2.offset.y) continue;

		if (IsInteracting(col1, col2))
			add_game_object_to_list(&list, go);
	}

	return list;
}

GameObjectList GetInteracts(GameObject* object)
{
	GameObjectList list;
	initialize_game_object_list(&list);

	BoxCollider col1 = object->collider;
	col1.size.x *= object->transform->scale.x;
	col1.size.y *= object->transform->scale.y;

	col1.offset.x *= object->transform->scale.x;
	col1.offset.y *= object->transform->scale.y;

	col1.offset.x += object->transform->position.x;
	col1.offset.y += object->transform->position.y;

#ifdef DEBUG_COLLISION
	if (renderable_state)
		DrawRectangleOnScreen((Rect) {
		col1.offset.x - (camera->position.x - camera->width / 2), col1.offset.y - (camera->position.y - camera->height / 2),
			col1.size.x, col1.size.y
	}, (Color) { 255, 0, 0, 255 });
#endif

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* go = GameObjects.List[i];
		if (go == NULL)
			continue;
		if (go == object) continue;

		BoxCollider col2 = go->collider;

		col2.offset.x *= go->transform->scale.x;
		col2.offset.y *= go->transform->scale.y;

		col2.size.x *= go->transform->scale.x;
		col2.size.y *= go->transform->scale.y;

		col2.offset.x += go->transform->position.x;
		col2.offset.y += go->transform->position.y;





		if (IsInteracting(col1, col2))
		{
			add_game_object_to_list(&list, go);

#ifdef DEBUG_COLLISION
			if (renderable_state)
				DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 255, 0, 0, 255 });
#endif
		}
		else
		{
#ifdef DEBUG_COLLISION
			if (renderable_state)

				DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 0, 255, 0, 255 });
#endif
		}

			
	}

	return list;
}


GameObjectList GetInteractsExceptLayer(GameObject* object, int layer_mask)
{
	GameObjectList list;
	initialize_game_object_list(&list);

	BoxCollider col1 = object->collider;
	col1.size.x *= object->transform->scale.x;
	col1.size.y *= object->transform->scale.y;

	col1.offset.x *= object->transform->scale.x;
	col1.offset.y *= object->transform->scale.y;

	col1.offset.x += object->transform->position.x;
	col1.offset.y += object->transform->position.y;

#ifdef DEBUG_COLLISION
	if (renderable_state)

		DrawRectangleOnScreen((Rect) {
		col1.offset.x - (camera->position.x - camera->width / 2), col1.offset.y - (camera->position.y - camera->height / 2),
			col1.size.x, col1.size.y
	}, (Color) { 255, 0, 0, 255 });
#endif

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* go = GameObjects.List[i];

		if (go == NULL)
			continue;

		if (go == object) continue;

		//int objLayerMask = (1 << go->layer);

		if ((go->layer & layer_mask)) continue;


		BoxCollider col2 = go->collider;
		col2.offset.x *= go->transform->scale.x;
		col2.offset.y *= go->transform->scale.y;

		col2.size.x *= go->transform->scale.x;
		col2.size.y *= go->transform->scale.y;

		col2.offset.x += go->transform->position.x;
		col2.offset.y += go->transform->position.y;




		if (IsInteracting(col1, col2))
		{
			add_game_object_to_list(&list, go);

#ifdef DEBUG_COLLISION
			if (renderable_state)

			DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 255, 0, 0, 255 });
#endif
		}
		else
		{
#ifdef DEBUG_COLLISION
			if (renderable_state)

			DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 0, 255, 0, 255 });
#endif
		}


	}

	return list;
}


GameObjectList GetInteractsOnlyLayer(GameObject* object, int layer_mask)
{
	GameObjectList list;
	initialize_game_object_list(&list);

	BoxCollider col1 = object->collider;

	col1.size.x *= object->transform->scale.x;
	col1.size.y *= object->transform->scale.y;

	col1.offset.x *= object->transform->scale.x;
	col1.offset.y *= object->transform->scale.y;

	col1.offset.x += object->transform->position.x;
	col1.offset.y += object->transform->position.y;

#ifdef DEBUG_COLLISION
	if (renderable_state)

		DrawRectangleOnScreen((Rect) {
		col1.offset.x - (camera->position.x - camera->width / 2), col1.offset.y - (camera->position.y - camera->height / 2),
			col1.size.x, col1.size.y
	}, (Color) { 255, 0, 0, 255 });
#endif

	for (int i = 0; i < GameObjects.Count; i++)
	{
		GameObject* go = GameObjects.List[i];
		if (go == NULL)
			continue;
		if (go == object) continue;

		//int objLayerMask = (1 << go->layer);

		if (!(go->layer & layer_mask)) continue;

		BoxCollider col2 = go->collider;
		col2.offset.x *= go->transform->scale.x;
		col2.offset.y *= go->transform->scale.y;

		col2.size.x *= go->transform->scale.x;
		col2.size.y *= go->transform->scale.y;

		col2.offset.x += go->transform->position.x;
		col2.offset.y += go->transform->position.y;




		if (IsInteracting(col1, col2))
		{
			add_game_object_to_list(&list, go);

#ifdef DEBUG_COLLISION
			if (renderable_state)

				DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 255, 0, 0, 255 });
#endif
		}
		else
		{
#ifdef DEBUG_COLLISION
			if (renderable_state)

				DrawRectangleOnScreen((Rect) {
				col2.offset.x - (camera->position.x - camera->width / 2), col2.offset.y - (camera->position.y - camera->height / 2),
					col2.size.x, col2.size.y
			}, (Color) { 0, 255, 0, 255 });
#endif
		}


	}

	return list;
}


Vector2 collider_center(GameObject* go)
{
	Vector2 center = { go->transform->position.x + (go->collider.size.x / 2 + go->collider.offset.x) * go->transform->scale.x,go->transform->position.y + (go->collider.size.y / 2 + go->collider.offset.y) * go->transform->scale.y };

	return center;
}

Vector2 collider_left_down(GameObject* go)
{
	Vector2 left_down = { go->transform->position.x + go->collider.offset.x * go->transform->scale.x, go->transform->position.y + (go->collider.size.y + go->collider.offset.y) * go->transform->scale.y };

	return left_down;
}

Vector2 collider_left_up(GameObject* go)
{
	Vector2 left_up = { go->transform->position.x + go->collider.offset.x * go->transform->scale.x, go->transform->position.y + (go->collider.offset.y) * go->transform->scale.y };

	return left_up;
}

Vector2 collider_right_down(GameObject* go)
{
	Vector2 right_down = { go->transform->position.x+(go->collider.offset.x + go->collider.size.x) * go->transform->scale.x, go->transform->position.y+(go->collider.size.y + go->collider.offset.y) * go->transform->scale.y };

	return right_down;
}

Vector2 collider_right_up(GameObject* go)
{
	Vector2 right_up = { go->transform->position.x+(go->collider.offset.x + go->collider.size.x) * go->transform->scale.x, go->transform->position.y + (go->collider.offset.y) * go->transform->scale.y };

	return right_up;
}

Vector2 collider_size(GameObject* go)
{
	Vector2 size;

	size.x = (go->collider.size.x + go->collider.offset.x) * go->transform->scale.x;
	size.y = (go->collider.size.y + go->collider.offset.y) * go->transform->scale.y;

	return size;
}


GameObject* is_on_platform(GameObject* self, int platform_layermask, float tolerance)
{

	GameObjectList list = GetInteractsOnlyLayer(self, platform_layermask);

	for (int i = 0; i < list.Count; i++)
	{
		GameObject* go = list.List[i];

		if (go != NULL)
		{
			Vector2 col_left_down = collider_left_down(self);

			Vector2 player_left_down = col_left_down;

			Vector2 ground_left_up = collider_left_up(go);

			if (float_compare(player_left_down.y, ground_left_up.y, tolerance)) // eðer ayaðýn platformun üstündeyse
			{
				self->transform->position.y = ground_left_up.y - collider_size(self).y;

				return go;
			}

		}
	}

	return NULL;
}

GameObject* is_on_something(GameObject* self, int layermask, float tolerance)
{

	GameObjectList list = GetInteractsOnlyLayer(self, layermask);

	for (int i = 0; i < list.Count; i++)
	{
		GameObject* go = list.List[i];

		if (go != NULL)
		{
			Vector2 col_left_down = collider_left_down(self);

			Vector2 player_left_down = col_left_down;

			Vector2 ground_left_up = collider_left_up(go);

			if (float_compare(player_left_down.y, ground_left_up.y, tolerance)) // eðer ayaðýn platformun üstündeyse
			{
				return go;
			}

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

		if (list.List[i] == except || list.List[i]->collider.is_trigger)
			return false;

		result = true;
		break;
	}


	return result;
}