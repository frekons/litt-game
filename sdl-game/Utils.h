#pragma once

//#ifndef UTILS_H
//#define UTILS_H

#include "Structs.h"

Point vec2_lerp(Point point1, Point point2, float time);

void* create_thread(void* func, void* parameter);

//#endif