#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"
#include "stack.h"

/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 circle: add a circle to the edge matrix - 
	    takes 3 arguments (cx, cy, r)
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         sphere: add a sphere to the edge matrix - 
	    takes 3 arguemnts (cx, cy, r)
         torus: add a torus to the edge matrix - 
	    takes 4 arguments (cx, cy, r1, r2)
         box: add a rectangular prism to the edge matrix - 
	    takes 6 arguemnts (x, y, z, width, height, depth)
	 clear: clear the currnt edge matrix -
	    takes 0 arguments
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 xrotate: create an x-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 yrotate: create an y-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 zrotate: create an z-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * pm,
                  screen s) {

  FILE *f;
  char line[256];
  struct matrix * tmp;
  double angle;
  color c;
  struct stack * stk = new_stack();
  
  c.red = 255;
  c.green = 255;
  c.blue = 255;
  
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);
    double x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
   
    
    if ( strncmp(line, "line", strlen(line)) == 0 ) {
      //      printf("LINE!\n");
      fgets(line, 255, f);
      //      printf("\t%s", line);
      //line[strlen(line)-1]='\0';
      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &x1, &y1, &z1);
      add_edge(pm, x, y, z, x1, y1, z1);
      matrix_mult(stk->data[stk->top], pm);
      draw_lines(pm, s, c);
      clear_matrix(pm);
      // printf( "%lf %lf %lf %lf %lf %lf\n", x, y, z, x1, y1, z1);
    }
    else if ( strncmp(line, "circle", strlen(line)) == 0 ) {
      //printf("CIRCLE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      add_circle(pm, x, y, z, 0.01);
      matrix_mult(stk->data[stk->top], pm);
      draw_lines(pm, s, c);
      clear_matrix(pm);
      //printf( "%lf %lf %lf\n", x, y, z);
    }    
    else if ( strncmp(line, "bezier", strlen(line)) == 0 ) {
      //printf("BEZIER\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
	     &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
      add_curve(pm, x1, y1, x2, y2, x3, y3, x4, y4, 0.01, BEZIER_MODE );
      matrix_mult(stk->data[stk->top], pm);
      draw_lines(pm, s, c);
      clear_matrix(pm);
      //printf( "%lf %lf %lf\n", x, y, z);
    }    
    else if ( strncmp(line, "hermite", strlen(line)) == 0 ) {
      //printf("HERMITE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
	     &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
      add_curve(pm, x1, y1, x2, y2, x3, y3, x4, y4, 0.01, HERMITE_MODE );
      matrix_mult(stk->data[stk->top], pm);
      draw_lines(pm, s, c);
      clear_matrix(pm);
      //printf( "%lf %lf %lf\n", x, y, z);
    }
    else if ( strncmp(line, "box", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &x1, &y1, &z1);
      add_box(pm, x, y, z, x1, y1, z1);
      matrix_mult(stk->data[stk->top], pm);
      draw_polygons(pm, s, c);
      clear_matrix(pm);
      // printf( "%lf %lf %lf %lf %lf %lf\n", x, y, z, x1, y1, z1);
    }
    else if (strncmp(line, "sphere", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      add_sphere(pm, x, y, z, 10);
      matrix_mult(stk->data[stk->top], pm);
      draw_polygons(pm, s, c);
      clear_matrix(pm);
      //printf( "%lf %lf %lf\n", x, y, z);
    }
    else if (strncmp(line, "torus", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf", &x, &y, &z, &z1);
      add_torus(pm, x, y, z, z1, 10);
      matrix_mult(stk->data[stk->top], pm);
      draw_polygons(pm, s, c);
      clear_matrix(pm);
      //printf( "%lf %lf %lf\n", x, y, z);
    }
    else if ( strncmp(line, "scale", strlen(line)) == 0 ) {
      //printf("SCALE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      tmp = make_scale(x, y, z);
      matrix_mult(stk->data[stk->top], tmp);
      copy_matrix(tmp, stk->data[stk->top]);
      //print_matrix(transform);
    }
    else if ( strncmp(line, "translate", strlen(line)) == 0 ) {
      //printf("TRANSLATE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      tmp = make_translate(x, y, z);
      matrix_mult(stk->data[stk->top], tmp);
      copy_matrix(tmp, stk->data[stk->top]);
      //print_matrix(transform);
    }
    else if ( strncmp(line, "xrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotX( angle);
      matrix_mult(stk->data[stk->top], tmp);
      copy_matrix(tmp, stk->data[stk->top]);
    }
    else if ( strncmp(line, "yrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotY( angle);
      matrix_mult(stk->data[stk->top], tmp);
      copy_matrix(tmp, stk->data[stk->top]);
    }
    else if ( strncmp(line, "zrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotZ( angle);
      matrix_mult(stk->data[stk->top], tmp);
      copy_matrix(tmp, stk->data[stk->top]);
    }
    else if( strncmp(line, "push", strlen(line)) == 0){
      push(stk);
    }
    else if( strncmp(line, "pop", strlen(line)) == 0 ){
      pop(stk);
    }
    else if ( strncmp(line, "ident", strlen(line)) == 0 ) {
      ident(transform);
    }
    else if ( strncmp(line, "apply", strlen(line)) == 0 ) {
      //printf("APPLY!\n");
      //print_matrix( transform );
      //      print_matrix(pm);
      //matrix_mult(transform, pm);
    }
    else if ( strncmp(line, "display", strlen(line)) == 0 ) {
      //clear_screen(s);
      //draw_polygons(pm, s, g);
      display(s);
    }
    else if ( strncmp(line, "save", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      // line[strlen(line)-1] = '\0';
      //clear_screen(s);
      //draw_polygons(pm, s, g);
      save_extension(s, line);
    }
    else if ( strncmp(line, "clear", strlen(line)) == 0 ) {
      clear_matrix(pm);
    }
    else if ( strncmp(line, "quit", strlen(line)) == 0 ) {
      return;
    }
    else if (strncmp(line, "color", strlen(line)) == 0){
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x1, &y1, &z1);
      c.red = x1;
      c.green = y1;
      c.blue= z1;
    }
    else if ( line[0] != '#' ) {
      printf("Invalid command\n");
    }
  }
  
  free_matrix(tmp);
  free_stack(stk);
  fclose(f);
  //printf("END PARSE\n");
}

  
