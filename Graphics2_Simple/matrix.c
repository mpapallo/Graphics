#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

/*-------------- struct matrix *new_matrix() --------------
Inputs:  int rows
         int cols 
Returns: 

Once allocated, access the matrix as follows:
m->m[r][c]=something;
if (m->lastcol)... 
*/
struct matrix *new_matrix(int rows, int cols) {

  double **tmp;
  int i;
  struct matrix *m;

  tmp = (double **)malloc(rows * sizeof(double *));
  for (i=0;i<rows;i++) {
      tmp[i]=(double *)malloc(cols * sizeof(double));
    }

  m=(struct matrix *)malloc(sizeof(struct matrix));
  m->m=tmp;
  m->rows = rows;
  m->cols = cols;
  m->lastcol = 0;

  return m;

}


/*-------------- void free_matrix() --------------
Inputs:  struct matrix *m 
Returns: 

1. free individual rows
2. free array holding row pointers
3. free actual matrix
*/
void free_matrix(struct matrix *m) {

  int i;
  for (i=0;i<m->rows;i++) {
      free(m->m[i]);
    }
  free(m->m);
  free(m);

}


/*======== void grow_matrix() ==========
Inputs:  struct matrix *m
         int newcols 
Returns: 

Reallocates the memory for m->m such that it now has
newcols number of collumns
====================*/
void grow_matrix(struct matrix *m, int newcols) {
  
  int i;
  for (i=0;i<m->rows;i++) {
      m->m[i] = realloc(m->m[i],newcols*sizeof(double));
  }
  m->cols = newcols;

}


/*-------------- void print_matrix() --------------
Inputs:  struct matrix *m 
Returns: 

print the matrix
*/
void print_matrix(struct matrix *m) {
  
  printf("\n");
  int r,c;
  for (r = 0; r < m->rows; r ++){
    for (c = 0; c < m->cols; c ++){
      printf("%f ", m->m[r][c]);
    }
    printf("\n");
  }

}


/*-------------- void ident() --------------
Inputs:  struct matrix *m <-- assumes m is a square matrix
Returns: 

turns m in to an identity matrix
*/
void ident(struct matrix *m) {

  int r, c;
  for (r = 0; r < 4; r ++){
    for (c = 0; c < 4; c ++){
      if (c == r){
	m->m[r][c] = 1;
      }else{
	m->m[r][c] = 0;
      }
    }
  }

}


/*-------------- void scalar_mult() --------------
Inputs:  double x
         struct matrix *m 
Returns: 

multiply each element of m by x
*/
void scalar_mult(double x, struct matrix *m) {

  int c;
  for (c = 0; c < m->lastcol; c++){
    m->m[0][c] = m->m[0][c] * x;
    m->m[1][c] = m->m[1][c] * x;
    m->m[2][c] = m->m[2][c] * x;
  }

}


/*-------------- void matrix_mult() --------------
Inputs:  struct matrix *a
         struct matrix *b 
Returns: 

a*b -> b
*/
void matrix_mult(struct matrix *a, struct matrix *b) {
  
  struct matrix *new;
  new = new_matrix(4, b->lastcol);

  int r, c;
  for (r = 0; r < 4; r ++){
    for (c = 0; c < b->lastcol; c ++){
      new->m[r][c] = a->m[r][0] * b->m[0][c] + a->m[r][1] * b->m[1][c] + a->m[r][2] * b->m[2][c] + a->m[r][3] * b->m[3][c];
      
    }
  }

  copy_matrix(new, b);

}


/*-------------- void copy_matrix() --------------
Inputs:  struct matrix *a
         struct matrix *b 
Returns: 

copy matrix a to matrix b
*/
void copy_matrix(struct matrix *a, struct matrix *b) {

  int r, c;
  for (r=0; r < a->rows; r++) 
    for (c=0; c < a->cols; c++)  
      b->m[r][c] = a->m[r][c]; 
 
}

/*======== struct matrix * make_translate() ==========
Inputs:  int x
         int y
         int z 
Returns: The translation matrix created using x, y and z 
as the translation offsets.
====================*/
struct matrix * make_translate(double x, double y, double z) {
 
  struct matrix *translate;
  translate = new_matrix(4, 4);
  ident(translate);
  translate->m[0][3] = x;
  translate->m[1][3] = y;
  translate->m[2][3] = z;

  return translate;

}

/*======== struct matrix * make_scale() ==========
Inputs:  int x
         int y
         int z 
Returns: The translation matrix creates using x, y and z
as the scale factors
====================*/
struct matrix * make_scale(double x, double y, double z) {
  
  struct matrix *scale;
  scale = new_matrix(4, 4);
  ident(scale);
  scale->m[0][0] = x;
  scale->m[1][1] = y;
  scale->m[2][2] = z;

  return scale;

}

/*======== struct matrix * make_rotX() ==========
Inputs:  double theta

Returns: The rotation matrix created using theta as the 
angle of rotation and X as the axis of rotation.
====================*/
struct matrix * make_rotX(double theta) {
  
  double rad = theta * (M_PI / 180);

  struct matrix *rot;
  rot = new_matrix(4, 4);
  ident(rot);
  rot->m[1][1] = cos(rad);
  rot->m[1][2] = -sin(rad);
  rot->m[2][1] = sin(rad);
  rot->m[2][2] = cos(rad);

  return rot;
  
}

/*======== struct matrix * make_rotY() ==========
Inputs:  double theta

Returns: The rotation matrix created using theta as the 
angle of rotation and Y as the axis of rotation.
====================*/
struct matrix * make_rotY(double theta) {

  double rad = theta * (M_PI / 180);

  struct matrix *rot;
  rot = new_matrix(4, 4);
  ident(rot);
  rot->m[0][0] = cos(rad);
  rot->m[0][2] = -sin(rad);
  rot->m[2][0] = sin(rad);
  rot->m[2][2] = cos(rad);

  return rot;

}

/*======== struct matrix * make_rotZ() ==========
Inputs:  double theta

Returns: The rotation matrix created using theta as the 
angle of rotation and Z as the axis of rotation.
====================*/
struct matrix * make_rotZ(double theta) {

  double rad = theta * (M_PI / 180);

  struct matrix *rot;
  rot = new_matrix(4, 4);
  ident(rot);
  rot->m[0][0] = cos(rad);
  rot->m[0][1] = -sin(rad);
  rot->m[1][0] = sin(rad);
  rot->m[1][1] = cos(rad);

  return rot;

}
