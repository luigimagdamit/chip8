#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define COLS 64
#define ROWS 32

#define POS "\u25A0"
#define NEG " "
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}
void render(int *canvas) {
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
  for(int i = 0; i < ROWS*COLS; i++) {
    if(canvas[i] == 0) {
      printf(NEG);
    } else {
      printf(POS);
    }

    if((i+1) % COLS == 0 && i != 0) {
      printf("\n");
    }
  }
}
void setPixel(int *canvas, int x, int y) { 
  if(x > COLS) {
    x-=COLS;
  } else if (x < 0) {
    x+=COLS;
  }

  if(y > ROWS) {
    y-=ROWS;
  } else if (y < 0) {
    y+=ROWS;
  }
  int pixelLoc = x + (y * COLS);
  canvas[pixelLoc] ^= 1;
}
int main() {
  printf("\e[?25l");
  int c[ROWS*COLS] = {0};
  for(int i = 0; i < ROWS; i++) {
    for(int j = 0; j < COLS; j++) {
      setPixel(c, j, i);
      render(c);
      setPixel(c, j, i);
      delay(1);
    }
  }
  render(c);
  printf("\e[?25h");
  return 0;
}
