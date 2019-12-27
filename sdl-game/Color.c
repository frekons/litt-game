#include "Colors.h" 

Uint32 BLACK = 0xFF000000;
Uint32 WHITE = 0xFFFFFFFF;

Color Player = { 25, 0, 255, 255 };

Color Black = { 0, 0, 0, 255 };
Color White =  { 255, 255, 255, 255 };
Color Red =  { 255, 0, 0, 255 };
Color Invisible =  { 0, 0, 0, 0 };

Color Bat =  { 255, 0, 16, 255 };
Color Skeleton =  { 193, 0, 12, 255 };
Color Archer =  { 130, 0, 8, 255 };
Color Boss =  { 0, 0, 0, 255 };

Color Trap1 =  { 255, 0, 242, 255 };
Color Trap2 =  { 255, 0, 168, 255 };

Color Dirt =  { 158, 92, 56, 255 };
Color Grass =  { 0, 130, 26, 255 };

Color Stone1 =  { 188, 188, 188, 255 };
Color Stone2 =  { 155, 155, 155, 255 };
Color Stone3 =  { 119, 119, 119, 255 };
Color Stone4 =  { 96, 96, 96, 255 };
Color Stone5 =  { 61, 61, 61, 255 };

void init_colors() {

}

bool compare_colors(Color color1, Color color2) {
	return (color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a) ? true : false;
}