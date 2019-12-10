#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>


char** split(char* str, char* delimiter, int* size);

void wait_after_input();

void wait();



#endif // !UTILS_H
