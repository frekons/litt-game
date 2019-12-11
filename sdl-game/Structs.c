#include "Structs.h"


Rect create_rect(int x, int y, int w, int h)
{
	Rect rect = { x,y,w,h };

	return rect;
}


Color create_color(int r, int g, int b, int a) {

	Color color = { r,g,b,a };

	return color;
}
