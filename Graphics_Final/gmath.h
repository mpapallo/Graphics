#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"

struct vertex{
  int c[3];
  double n[3];
};

double * calculate_normal( double a1, double a2, double a3,
			   double b1, double b2, double b3 );
double * calculate_surface_normal( struct matrix *points, int i );
double calculate_dot( struct matrix *points, int i );
double dot_product( double * v1, double * v2 );
double * normalize( double * v );
double diffuse_mult( double * normal, double * light );
double specular_mult( double * normal, double * light, double * view );


#endif
