#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"

int main(){

  screen s;
  color c;

  c.red = 0;
  c.green = 0;
  c.blue = MAX_COLOR;

  int ctr = 0;
  while (ctr <= 200){
    draw_line(0, ctr, XRES-1, YRES-1, s, c);
    ctr ++;
    c.red = ctr % MAX_COLOR;
  }

  c.red = 0;
  ctr = 0;
  while (ctr <= 200){
    draw_line(XRES-1, YRES-1, ctr, 0, s, c);
    ctr ++;
    c.green = ctr % MAX_COLOR;
  }

  c.green = 0;
  ctr = 0;
  while (ctr < XRES){
    draw_line(0, 201, ctr, YRES-1, s, c);
    ctr ++;
    c.green = ctr % MAX_COLOR;
  }

  c.green = 0;
  ctr = 0;
  while (ctr < YRES){
    draw_line(201, 0, XRES-1, ctr, s, c);
    ctr ++;
    c.red = ctr % MAX_COLOR;
  }
  
  
  save_extension(s, "pic.png");
  display(s);
  
  return 0;
}
