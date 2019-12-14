#pragma once

//#ifndef UTILS_H
//#define UTILS_H

#include "Structs.h"

Point point_lerp(Point point1, Point point2, float time);

Vector2 vec2_sum(Point vec1, Vector2 vec2);

Vector2 vec2_minus(Vector2 vec1, Vector2 vec2);

void* create_thread(void* func, void* parameter);

float clamp(float value, float min, float max);

float sign(float value);

bool float_compare(float one, float two, float tolerance);


void destroy_after(GameObject* object, float after_time);




//#endif