#include <stdio.h>
#include <unistd.h>

#define COLS 64
#define ROWS 32

struct RendererInfo {
    int cols;
    int rows;
    int scale;
};

struct Canvas {
    int pixels[COLS*ROWS];
};

int setPixel(int x, int y, int canvas[]) {
    int x_local = x;
    int y_local = y;
    if(x_local > COLS) {
        x_local -= COLS;
    } else if (x_local < 0) {
        x_local += COLS;
    }
    if ( y_local > ROWS) {
        y_local -= ROWS;
    }

    int pixelLoc = x_local + (y_local * COLS);
    canvas[pixelLoc] ^= 1;
    return canvas[pixelLoc];
}

void clear(int canvas[]) {
    for(int i  = 0; i < ROWS*COLS; i++) {
        canvas[i] = 0;
    }
}


void render(int canvas[]) {
    const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    for(int i = 0; i < ROWS * COLS; i++) {
        if(i % COLS == 0 && canvas[i] == 1){
            printf("\u25A0\n");
        } 
        else if (i % COLS == 0 && canvas[i] == 0) {
            printf("x\n");
        }
        else if (i % COLS != 0 && canvas[i] == 1) {
            printf("\u25A0");
        }
        else if (i % COLS != 0 && canvas[i] == 0) {
            printf("x");
        }
    }

}
void instantiateScreen(int canvas[]) {
    for(int i = 0; i < ROWS * COLS; i++) {
        canvas[i] = 0;
    }
}