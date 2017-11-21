#include <stdio.h>
#include <stdlib.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"

float slope(int x0, int y0, int x1, int y1){
  return (1.0*(y1 - y0)) / (1.0*(x1 - x0));
}

//Insert your line algorithm here
void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {

  float m = slope(x0, y0, x1, y1);
  //printf("%f\n", m);

  if (x0 > x1){//switch 'em
    int tempx = x0;
    int tempy = y0;
    x0 = x1;
    y0 = y1;
    x1 = tempx;
    y1 = tempy;
  }
  int x = x0;
  int y = y0;
  int A = y1 - y0;
  int B = x0 - x1;
  int d;

  // 0 < m < 1
  //Octant I or V
  if (m >= 0 &&  m <= 1){
    d = 2 * A + B;
    while (x <= x1){
      plot(s, c, x, y);
      if (d > 0){
	y ++;
	d += 2 * B;
      }
      x ++;
      d += 2 * A;
    }
  }

  // m > 1
  //Octant II or VI
  else if (m > 1){
    d = A + 2 * B;
    while (y <= y1){
      plot(s, c, x, y);
      if (d < 0){
	x ++;
	d += 2 * A;
      }
      y ++;
      d += 2 * B;
    }
  }

  //-1 < m < 0
  //Octant IV or VIII
  else if (m >= -1 && m < 0){
    d = 2 * A - B;
    while (x <= x1){
      plot(s, c, x, y);
      if (d < 0){
	y --;
	d -= 2 * B;
      }
      x ++;
      d += 2 * A;
    }
  }
  
  //m < -1
  //Octant III or VII
  else if (m < -1){
    d = A - 2 * B;
    while (y >= y1){
      plot(s, c, x, y);
      if (d > 0){
	x ++;
	d += 2 * A;
      }
      y --;
      d -= 2 * B;
    }
  }
  
}
