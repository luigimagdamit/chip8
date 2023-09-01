#include <stdio.h>
#include "renderer.h"

int main() {
    struct Canvas canvas;
    instantiateScreen(canvas.pixels);
    setPixel(0, 0, canvas.pixels);
    setPixel(63, 0, canvas.pixels);
    setPixel(0, 31, canvas.pixels);
    setPixel(63, 31, canvas.pixels);
    render(canvas.pixels);
    return 0;
}