#pragma once

//#ifndef UTILS_H
//#define UTILS_H

#include "Structs.h"

float vec2_magnitude(Vector2 point);

Point point_lerp(Point point1, Point point2, float time);

Vector2 vec2_lerp(Vector2 point1, Vector2 point2, float time);

Vector2 vec2_movetowards(Vector2 point1, Vector2 point2, float speed);

float float_lerp(float value1, float value2, float time); // time is between 0, 1

Point point_sum(Point vec1, Vector2 vec2);

Vector2 vec2_multiplier(Vector2 vec1, float multiplier);

Vector2 vec2_sum(Point vec1, Vector2 vec2);

Vector2 vec2_minus(Vector2 vec1, Vector2 vec2);

void* create_thread(void* func, void* parameter);

float clamp(float value, float min, float max);

float sign(float value);

bool float_compare(float one, float two, float tolerance);


void destroy_after(GameObject* object, float after_time);




//#endif