#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"

void draw_line(int x0, int y0, int x1, int y1, screen s, color c);
void add_point( struct matrix * points, int x, int y, int z);
void add_edge( struct matrix * points, 
	       int x0, int y0, int z0, 
	       int x1, int y1, int z1);
void draw_lines( struct matrix * points, screen s, color c);

#endif
