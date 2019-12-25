#include "Colors.h" 

void init_colors() {
	BLACK = 0xFF000000;
	WHITE = 0xFFFFFFFF;

	Player = (Color) {25,0,255,255};

	Black = (Color) { 0,0,0,255 };
	White = (Color) { 255,255,255,255 };
	Red = (Color){ 255,0,0,255 };
	Invisible = (Color) { 0,0,0,0 };

	Bat = (Color) { 255,0,16,255 };
	Skeleton = (Color) { 193,0,12,255 };
	Archer = (Color) { 130,0,8,255 };
	Boss = (Color) { 0,0,0,255 };

	Trap1 = (Color) {255,0,242,255};
	Trap2 = (Color) {255,0,168,255};

	Dirt = (Color) { 158,92,56,255 };
	Grass = (Color) { 0,130,26,255 };

	Stone1 = (Color) { 188,188,188,255 };
	Stone2 = (Color) { 155,155,155,255 };
	Stone3 = (Color) { 119,119,119,255 };
	Stone4 = (Color) { 96,96,96,255 };
	Stone5 = (Color) { 61,61,61,255 };
}

bool compare_colors(Color color1, Color color2) {
	return (color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a) ? true : false;
}