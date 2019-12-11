#include "Draws.h"

#include "Globals.h"

void DrawRectangle(Rect rect, Color color) 
{
	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderDrawRect(renderer, &rect);

	set_render_color(temp_color);
}

void DrawFilledRectangle(Rect rect, Color color)
{
	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderDrawRectF(renderer, &rect);

	set_render_color(temp_color);
}

void DrawLine(Point A, Point B, Color color)
{
	Color temp_color = get_render_color();

	set_render_color(color);

	SDL_RenderDrawLine(renderer, A.x, A.y, B.x, B.y);

	set_render_color(temp_color);
}