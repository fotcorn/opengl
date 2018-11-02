#include "program.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
    Program program;
    program.init();
    program.mainLoop();
    return 0;
}
