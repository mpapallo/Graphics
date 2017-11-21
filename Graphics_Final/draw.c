
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"
#include "symtab.h"

/*======== void add_polygon() ==========
Inputs:   struct matrix *surfaces
         double x0
         double y0
         double z0
         double x1
         double y1
         double z1
         double x2
         double y2
         double z2  
Returns: 
Adds the vertices (x0, y0, z0), (x1, y1, z1)
and (x2, y2, z2) to the polygon matrix. They
define a single triangle surface.

04/16/13 13:05:59
jdyrlandweaver
====================*/
void add_polygon( struct matrix *polygons, 
		  double x0, double y0, double z0, 
		  double x1, double y1, double z1, 
		  double x2, double y2, double z2 ) {
  add_point(polygons, x0, y0, z0);
  add_point(polygons, x1, y1, z1);
  add_point(polygons, x2, y2, z2);
}

double distance(double x0, double y0, double z0, double x1, double y1, double z1){
  double sum;
  sum = (x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0) + (z1 - z0)*(z1 - z0);
  return sqrt(sum);
}

void scanline_convert( struct matrix *polygons, zbuffer zbuf, int i, screen s, color c ){

  printf("Scaline convert\n");
  double xb, yb, zb, xt, yt, zt, xm, ym, zm;
  double bt_inc, bm_inc, mt_inc;
  double bt_zinc, bm_zinc, mt_zinc;
  int magic_num, b, j;

  //determine top, middle, and bottom
  xt=polygons->m[0][i];
  yt=polygons->m[1][i];
  zt=polygons->m[2][i];
  
  xb=polygons->m[0][i];
  yb=polygons->m[1][i];
  zb=polygons->m[2][i];
  
  b=0;
  magic_num=0;
  
  for (j=1;j<3;j++){
    if (polygons->m[1][i+j] >= yt){
      xt=polygons->m[0][i+j];
      yt=polygons->m[1][i+j];
      zt=polygons->m[2][i+j];
      b=j;
    }
  }
  magic_num+=b;
  b=0;
  for (j=1;j<3;j++){
    if (polygons->m[1][i+j] <= yb){
      xb=polygons->m[0][i+j];
      yb=polygons->m[1][i+j];
      zb=polygons->m[2][i+j];
      b=j;
    }
  }
  magic_num+=b;
  magic_num=3-magic_num;
  xm=polygons->m[0][i+magic_num];
  ym=polygons->m[1][i+magic_num];
  zm=polygons->m[2][i+magic_num];
  //finished determining top, middle, bottom   

  //delta0 and delta1; all inc values are positive
  bt_inc=(xt-xb)/(yt-yb);
  mt_inc=(xt-xm)/(yt-ym);
  bm_inc=(xm-xb)/(ym-yb);
  
  bm_zinc = (zm - zb) / (ym - yb); //distance(xb, yb, xm, ym);
  bt_zinc = (zt - zb) / (yt - yb); //distance(xb, yb, xt, yt);
  mt_zinc = (zt - zm) / (yt - ym); // distance(xm, ym, xt, yt);
  
  //printf("top (%f, %f) mid (%f, %f) bot (%f, %f)\n", xt, yt, xm, ym, xb, yb);
  
  double left_x  = xb;
  double right_x = xb;
  int passed_middle = 0;
  
  double left_z = zb;
  double right_z = zb;
  
  while (yb < yt){
    if (!passed_middle && yb >= ym){
      right_x = xm;
      right_z = zm;
      passed_middle = 1;
    }
    //printf("left_x %f, right_x %f, y %f left_z %f right_z %f\n", left_x, right_x, yb, left_z, right_z);
    draw_line(left_x, yb, left_z, right_x, yb, right_z, zbuf, s, c);
    left_x += bt_inc;
    left_z += bt_zinc;
    if (yb > ym){
      right_x += mt_inc;
      right_z += mt_zinc;
    }else{
      right_x += bm_inc;
      right_z += bm_zinc;
    }
    yb++;
  }  
}

/*======== void draw_polygons() ==========
Inputs:   struct matrix *polygons
          screen s
          color c  
Returns: 
Goes through polygons 3 points at a time, drawing 
lines connecting each points to create bounding
triangles

04/16/13 13:13:27
jdyrlandweaver
====================*/
void draw_polygons( struct matrix *polygons, zbuffer zbuf, screen s, color c, struct constants k, struct light * lights, int num_lights ) {

  printf("Draw polygons\n");
  int i;
  for( i=0; i < polygons->lastcol-2; i+=3 ) {
    
    if ( calculate_dot( polygons, i ) < 0 ) {

      //CALCULATE COLOR OF POLYGON
      double * Iamb;
      Iamb = (double *)malloc(3*sizeof(double));
      double * Idif;
      Idif = (double *)malloc(3*sizeof(double));
      double * Ispec;
      Ispec = (double *)malloc(3*sizeof(double));
      int lindex;
      //double point_l[3];
      double * point_l;
      point_l = (double *)malloc(3*sizeof(double));
      
      double * normal = (double *)malloc(3*sizeof(double));
      normal = calculate_surface_normal(polygons, i);
      
      printf("normal %f %f %f", normal[0], normal[1], normal[2]);

      //double view_vector[3];
      double * view_vector;
      view_vector = (double *)malloc(3*sizeof(double));

      view_vector[0] = lights[0].l[0];
      view_vector[1] = lights[0].l[1];
      view_vector[2] = lights[0].l[2];
      
      //Iamb = ambient color * constant of ambient reflection
      Iamb[ired] = lights[Ka].c[ired] * k.r[Ka];
      Iamb[igreen] = lights[Ka].c[igreen] * k.g[Ka];
      Iamb[iblue] = lights[Ka].c[iblue] * k.b[Ka];

      for (lindex = 1; lindex < num_lights; lindex++){

	//light src vector
	point_l[0] = lights[lindex].l[0];
	point_l[1] = lights[lindex].l[1];
	point_l[2] = lights[lindex].l[2];

	printf("dot prod %f\n", diffuse_mult(normal, point_l));

	//Idif = point light src color * constant of diffuse reflection * dot product of normalized light vector and normalized normal
	
	Idif[ired] = lights[lindex].c[ired] * k.r[Kd] * diffuse_mult(normal, point_l);
	Idif[igreen] = lights[lindex].c[igreen] * k.g[Kd] * diffuse_mult(normal, point_l);
	Idif[iblue] = lights[lindex].c[iblue] * k.b[Kd] * diffuse_mult(normal, point_l);
	
	//Ispec = point light src color * constant of specular reflection * that hot mess from my notes
	Ispec[ired] = lights[lindex].c[ired] * k.r[Ks] * specular_mult(normal, point_l, view_vector);
	Ispec[igreen] = lights[lindex].c[igreen] * k.g[Ks] * specular_mult(normal, point_l, view_vector);
	Ispec[iblue] = lights[lindex].c[iblue] * k.b[Ks] * specular_mult(normal, point_l, view_vector);

      }

      printf("please work...\n"); 
      c.red = (int)(Iamb[ired] + Idif[ired] + Ispec[ired]);
      if (c.red > 255){
	c.red = 255;
      }else if (c.red < 0){
	c.red = 0;
      }
      c.green = (int)(Iamb[igreen] + Idif[igreen] + Ispec[igreen]);
      if (c.green > 255){
	c.green = 255;
      }else if (c.green < 0){
	c.green = 0;
      }
      c.blue = (int)(Iamb[iblue] + Idif[iblue] + Ispec[iblue]);
      if (c.blue > 255){
	c.blue = 255;
      }else if (c.blue < 0){
	c.blue = 0;
      }

      printf("red %d green %d blue %d\n", c.red, c.green, c.blue);
      
      //DRAW BORDERS
      draw_line( polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 zbuf, s, c);
      draw_line( polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 zbuf, s, c);
      draw_line( polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 zbuf, s, c);

      printf("borders drawn\n");
      //FILL IN (scanline conversion)
      scanline_convert( polygons, zbuf, i, s, c);

      free(Iamb);
      free(Idif);
      free(Ispec);
      free(point_l);
      free(normal);
      
    }
  }

}

/*======== void add_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  adds all the points for a sphere with center 
  (cx, cy) and radius r.

  should call generate_sphere to create the
  necessary points

  jdyrlandweaver
  ====================*/
  void add_sphere( struct matrix * points, 
		 double cx, double cy, double cz, double r, 
		 int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps, num_points;
  double px0, px1, px2, px3;
  double py0, py1, py2, py3;
  double pz0, pz1, pz2, pz3;

  num_steps = MAX_STEPS / step;
  num_points = num_steps * (num_steps + 1);
  
  temp = new_matrix( 4, num_points);
  //generate the points on the sphere
  generate_sphere( temp, cx, cy, cz, r, step );

  int latStop, longStop, latStart, longStart;
  latStart = 0;
  latStop = num_steps;
  longStart = 0;
  longStop = num_steps;

  num_steps++;

  for ( lat = latStart; lat < latStop; lat++ ) {
    for ( longt = longStart; longt < longStop; longt++ ) {
      
      index = lat * num_steps + longt;

      px0 = temp->m[0][ index ];
      py0 = temp->m[1][ index ];
      pz0 = temp->m[2][ index ];
      
      px1 = temp->m[0][ (index + num_steps) % num_points ];
      py1 = temp->m[1][ (index + num_steps) % num_points ];
      pz1 = temp->m[2][ (index + num_steps) % num_points ];

      px3 = temp->m[0][ index + 1 ];
      py3 = temp->m[1][ index + 1 ];
      pz3 = temp->m[2][ index + 1 ];

      if (longt != longStop - 1) {
	px2 = temp->m[0][ (index + num_steps + 1) % num_points ];
	py2 = temp->m[1][ (index + num_steps + 1) % num_points ];
	pz2 = temp->m[2][ (index + num_steps + 1) % num_points ];
      }
      else {
	px2 = temp->m[0][ (index + 1) % num_points ];
	py2 = temp->m[1][ (index + 1) % num_points ];
	pz2 = temp->m[2][ (index + 1) % num_points ];
      }

      if (longt != 0)
	add_polygon( points, px0, py0, pz0, px1, py1, pz1, px2, py2, pz2 );
      if (longt != longStop - 1)
	add_polygon( points, px2, py2, pz2, px3, py3, pz3, px0, py0, pz0 );
    }
  }
}

/*======== void generate_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  sphere with center (cx, cy) and radius r

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_sphere( struct matrix * points, 
		      double cx, double cy, double cz, double r, 
		      int step ) {


  int circle, rotation;
  double x, y, z, circ, rot;

  int rotStart = step * 0;
  int rotStop = MAX_STEPS;
  int circStart = step * 0;
  int circStop = MAX_STEPS;
  
  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {
    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle <= circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = r * cos( M_PI * circ ) + cx;
      y = r * sin( M_PI * circ ) *
	cos( 2 * M_PI * rot ) + cy;
      z = r * sin( M_PI * circ ) *
	sin( 2 * M_PI * rot ) + cz;

      add_point( points, x, y, z);
    }
  }
}    


/*======== void add_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r1
	    double r2
	    double step  
  Returns: 

  adds all the points required to make a torus
  with center (cx, cy) and radii r1 and r2.

  should call generate_torus to create the
  necessary points

  03/22/12 13:34:03
  jdyrlandweaver
  ====================*/
void add_torus( struct matrix * points, 
		double cx, double cy, double cz, double r1, double r2, 
		int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps;
  
  num_steps = MAX_STEPS / step;

  temp = new_matrix( 4, num_steps * num_steps );
  //generate the points on the torus
  generate_torus( temp, cx, cy, cz, r1, r2, step );
  int num_points = temp->lastcol;

  int latStop, longtStop, latStart, longStart;
  latStart = 0;
  longStart = 0;
  latStop = num_steps;
  longtStop = num_steps;
  for ( lat = latStart; lat < latStop; lat++ )
    for ( longt = longStart; longt < longtStop; longt++ ) {

      index = lat * num_steps + longt;

      if ( longt != num_steps-1) {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps+1) % num_points],
		     temp->m[1][(index+num_steps+1) % num_points],
		     temp->m[2][(index+num_steps+1) % num_points],
		     temp->m[0][index+1],
		     temp->m[1][index+1],
		     temp->m[2][index+1] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+num_steps) % num_points + 1],
		     temp->m[1][(index+num_steps) % num_points + 1],
		     temp->m[2][(index+num_steps) % num_points + 1]);
      }
      else {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points],
		     temp->m[0][index+1-num_steps],
		     temp->m[1][index+1-num_steps],
		     temp->m[2][index+1-num_steps] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points]);
      }

    }
}

/*======== void generate_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  tarus with center (cx, cy) and radii r1 and r2

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_torus( struct matrix * points, 
		     double cx, double cy, double cz, double r1, double r2, 
		     int step ) {

  double x, y, z, circ, rot;
  int circle, rotation;

  double rotStart = step * 0;
  double rotStop = MAX_STEPS;
  double circStart = step * 0;
  double circStop = MAX_STEPS;

  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {

    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle < circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = cos( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cx;
      y = r1 * sin( 2 * M_PI * circ ) + cy;
      z = sin( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cz;

      add_point( points, x, y, z );
    }
  }
}

/*======== void add_box() ==========
  Inputs:   struct matrix * points
            double x
	    double y
	    double z
	    double width
	    double height
	    double depth
  Returns: 

  add the points for a rectagular prism whose 
  upper-left corner is (x, y, z) with width, 
  height and depth dimensions.

  jdyrlandweaver
  ====================*/
void add_box( struct matrix * polygons,
	      double x, double y, double z,
	      double width, double height, double depth ) {

  double x2, y2, z2;
  x2 = x + width;
  y2 = y - height;
  z2 = z - depth;
  //front
  add_polygon( polygons, 
	       x, y, z, 
	       x, y2, z,
	       x2, y2, z);
  add_polygon( polygons, 
	       x2, y2, z, 
	       x2, y, z,
	       x, y, z);
  //back
  add_polygon( polygons, 
	       x2, y, z2, 
	       x2, y2, z2,
	       x, y2, z2);
  add_polygon( polygons, 
	       x, y2, z2, 
	       x, y, z2,
	       x2, y, z2);
  //top
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y, z,
	       x2, y, z);
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y, z2,
	       x, y, z2);
  //bottom
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y2, z,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y2, z2,
	       x2, y2, z2);
  //right side
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y2, z,
	       x2, y2, z2);
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y, z2,
	       x2, y, z);
  //left side
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y2, z2,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y, z,
	       x, y, z2); 
}
  
/*======== void add_circle() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double y
	    double step  
  Returns: 


  03/16/12 19:53:52
  jdyrlandweaver
  ====================*/
void add_circle( struct matrix * points, 
		 double cx, double cy, 
		 double r, double step ) {
  
  double x0, y0, x, y, t;
  
  x0 = cx + r;
  y0 = cy;

  for ( t = step; t <= 1; t+= step ) {
    
    x = r * cos( 2 * M_PI * t ) + cx;
    y = r * sin( 2 * M_PI * t ) + cy;
    
    add_edge( points, x0, y0, 0, x, y, 0 );
    x0 = x;
    y0 = y;
  }

  add_edge( points, x0, y0, 0, cx + r, cy, 0 );
}

/*======== void add_curve() ==========
Inputs:   struct matrix *points
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type  
Returns: 

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix points

03/16/12 15:24:25
jdyrlandweaver
====================*/
void add_curve( struct matrix *points, 
		double x0, double y0, 
		double x1, double y1, 
		double x2, double y2, 
		double x3, double y3, 
		double step, int type ) {

  double x, y, t;
  struct matrix * xcoefs;
  struct matrix * ycoefs;
  
  //generate the coeficients
  if ( type == BEZIER_MODE ) {
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, BEZIER_MODE);
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, BEZIER_MODE);
  }

  else {
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, HERMITE_MODE);
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, HERMITE_MODE);
  }

  /*
  printf("a = %lf b = %lf c = %lf d = %lf\n", xcoefs->m[0][0],
         xcoefs->m[1][0], xcoefs->m[2][0], xcoefs->m[3][0]);
  */

  for (t=step; t <= 1; t+= step) {
    
    x = xcoefs->m[0][0] * t * t * t + xcoefs->m[1][0] * t * t
      + xcoefs->m[2][0] * t + xcoefs->m[3][0];

    y = ycoefs->m[0][0] * t * t * t + ycoefs->m[1][0] * t * t
      + ycoefs->m[2][0] * t + ycoefs->m[3][0];

    add_edge(points, x0, y0, 0, x, y, 0);
    x0 = x;
    y0 = y;
  }

  free_matrix(xcoefs);
  free_matrix(ycoefs);
}

/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {
  
  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][points->lastcol] = x;
  points->m[1][points->lastcol] = y;
  points->m[2][points->lastcol] = z;
  points->m[3][points->lastcol] = 1;

  points->lastcol++;
}

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}


/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, zbuffer zbuf, screen s, color c) {

  int i;
 
  if ( points->lastcol < 2 ) {
    
    printf("Need at least 2 points to draw a line!\n");
    return;
  }

  for ( i = 0; i < points->lastcol - 1; i+=2 ) {

    draw_line( points->m[0][i], points->m[1][i], points->m[2][i],
	       points->m[0][i+1], points->m[1][i+1], points->m[2][i+1], 
	       zbuf, s, c);
  /*
    //FOR DEMONSTRATION PURPOSES ONLY
    //draw extra pixels so points can actually be seen    
   
    draw_line( points->m[0][i]+1, points->m[1][i], 
	       points->m[0][i+1]+1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]+1, 
	       points->m[0][i+1], points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i], 
	       points->m[0][i+1]-1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]-1, 
	       points->m[0][i+1], points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]+1, 
	       points->m[0][i+1]+1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]+1, 
	       points->m[0][i+1]-1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]-1, 
	       points->m[0][i+1]-1, points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]-1, 
	       points->m[0][i+1]+1, points->m[1][i+1]-1, s, c);
  */

  } 	       
}

void draw_line(int x0, int y0, double z0, int x1, int y1, double z1, zbuffer zbuf, screen s, color c) {
 
  int x, y, d, dx, dy;
  double z, dz;
  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always draing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
  }

  //need to know dx, dy, dz for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;
  //dz = (z1 - z)/abs(y1 - y); //distance(x1, y1, z1, x, y, z);
  
  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {

    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      //dz = (z1 - z)/distance(x1, y1, z1, x, y, z);//abs(x1 - x);
      dz = (z1-z)/abs(x1-x); 
      while ( x <= x1 ) {
	plot(s, c, zbuf, x, y, z);
	if ( d < 0 ) {
	  x = x + 1;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  d = d + dy - dx;
	}
	z += dz;
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      //dz = (z1 - z)/distance(x1, y1, z1, x, y, z); //abs(y1 - y);
      dz = (z1-z)/abs(y1-y);
      while ( y <= y1 ) {
	plot(s, c, zbuf, x, y, z );
	//z += dz;
	if ( d > 0 ) {
	  y = y + 1;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  d = d + dy - dx;
	}
	z += dz;
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {
      d = dy + ( dx / 2 );
      //dz = (z1 - z)/distance(x1, y1, z1, x, y, z); //abs(x1 - x);
      dz = (z1 - z)/abs(x1 - x);
      while ( x <= x1 ) {
	plot(s, c, zbuf, x, y, z);
	//z += dz;
	if ( d > 0 ) {
	  x = x + 1;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  d = d + dy + dx;
	}
	z += dz;
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;
      //dz = (z1 - z)/distance(x1, y1, z1, x, y, z);//abs(y1 - y);
      dz = (z1 - z)/abs(y - y1);
      while ( y >= y1 ) {	
	plot(s, c, zbuf, x, y, z );
	//z += dz;
	if ( d < 0 ) {
	  y = y - 1;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  d = d + dy + dx;
	}
	z += dz;
      }
    }
  }
}

