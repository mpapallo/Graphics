#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"

int main() {

  screen s;
  color c;

  c.red = 50;
  c.green = 0;
  c.blue = MAX_COLOR;
  
  clear_screen(s);

  int x = XRES / 2;
  int y = YRES / 2;

  struct matrix *edges;
  struct matrix *transform;
  edges = new_matrix(4, 4);
  transform = new_matrix(4, 4);

  //Testing
  printf("\nEdges:\n");
  add_edge(edges, x+50, y+50, 50, x+50, y-50, 50);
  add_edge(edges, x+50, y-50, 50, x-50, y-50, 50);
  add_edge(edges, x-50, y-50, 50, x-50, y+50, 50);
  add_edge(edges, x-50, y+50, 50, x+50, y+50, 50);
  print_matrix(edges);

  printf("\nScalar Mult (0.25):\n");
  scalar_mult(0.25, edges);
  print_matrix(edges);
  printf("\nIdent Matrix:\n");
  ident(transform);
  print_matrix(transform);
  printf("\nMult by Identity:\n");
  matrix_mult(transform, edges);
  print_matrix(edges);
  printf("\nTest Translate (100, 100, 100):\n");
  transform = make_translate(100, 100, 100);
  print_matrix(transform);
  matrix_mult(transform, edges);
  print_matrix(edges);
  printf("\nTest Scale (1, 2, 3):\n");
  transform = make_scale(1, 2, 3);
  matrix_mult(transform, edges);
  print_matrix(edges);

  printf("\nTest RotX:\n");
  transform = make_rotX(5);
  matrix_mult(transform, edges);
  print_matrix(edges);

  //Image
  struct matrix *img;
  img = new_matrix(4, 4);

  add_edge(img, x+50, y+50, 50, x+50, y-50, 50);
  add_edge(img, x+50, y-50, 50, x-50, y-50, 50);
  add_edge(img, x-50, y-50, 50, x-50, y+50, 50);
  add_edge(img, x-50, y+50, 50, x+50, y+50, 50);
  
  int i;
  for (i = 0; i < 360; i ++){
    c. red = (c.red + 50) % MAX_COLOR;
    //c.green = i % MAX_COLOR;    
    transform = make_rotZ(-i);
    matrix_mult(transform, img);
    scalar_mult(0.9, img);
    transform = make_translate(100, 100, 100);
    matrix_mult(transform, img);
    draw_lines(img, s, c);
    transform = make_translate(-100, -100, -100);
    matrix_mult(transform, img);
  }
  
  free_matrix( transform );
  free_matrix( edges );

  save_extension(s, "matrix.png");
  display(s);

}
