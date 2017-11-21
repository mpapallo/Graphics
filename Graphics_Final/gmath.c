#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "gmath.h"



/*======== double * calculate_normal() ==========
  Inputs:   double ax
            double ay
	    double az
	    double bx
	    double by
	    double bz  
  Returns: A double arry of size 3 representing the 
           cross product of <ax, ay, az> and <bx, by, bz>

  04/17/12 16:46:30
  jonalf
  ====================*/
double * calculate_normal( double ax, double ay, double az,	
			   double bx, double by, double bz ) {
  
  double *normal;
  normal = (double *)malloc(3 * sizeof(double));

  normal[0] = ay*bz - az*by;
  normal[1] = az*bx - ax*bz;
  normal[2] = ax*by - ay*bx;

  return normal;
}

double * calculate_surface_normal( struct matrix *points, int i ) {

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );
  double normal_magnitude=sqrt(pow(normal[0],2) + pow(normal[1],2) + pow(normal[2],2));
  normal[0]=normal[0]/normal_magnitude;
  normal[1]=normal[1]/normal_magnitude;
  normal[2]=normal[2]/normal_magnitude;
  return normal;
}

/*======== double calculate_dot() ==========
  Inputs:   struct matrix *points
            int i  
  Returns: The dot product of a surface normal and
           a view vector
  
  calculates the dot product of the surface normal to
  triangle points[i], points[i+1], points[i+2] and a 
  view vector (use <0, 0, -1> to start.

  04/17/12 16:38:34
  jonalf
  ====================*/
double calculate_dot( struct matrix *points, int i ) {

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );

  //set up view vector
  vx = 0;
  vy = 0;
  vz = -1;

  //calculate dot product
  dot = normal[0] * vx + normal[1] * vy + normal[2] * vz;

  free(normal);  
  return dot;
}

double dot_product( double * v1, double * v2 ){
  return  v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

double * normalize( double * v ){
  double * new_v;
  double mag = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
  new_v[0] = v[0] / mag;
  new_v[1] = v[1] / mag;
  new_v[2] = v[2] / mag;
  return new_v;
}

double diffuse_mult( double * normal, double * light ){
  return dot_product( normalize(normal), normalize(light) );
}

double specular_mult( double * normal, double * light, double * view){
  double NdotL = dot_product(normal, light);
  double * tmp = (double *)malloc(3 * sizeof(double));
  tmp[0] = 2 * NdotL * normal[0] - light[0];
  tmp[1] = 2 * NdotL * normal[1] - light[1];
  tmp[2] = 2 * NdotL * normal[2] - light[2];

  double cos_theta = dot_product(tmp, view);
  double sharp = 4;
  cos_theta = pow(cos_theta, sharp);
  free(tmp);
  return cos_theta;
}
