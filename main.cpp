#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
    const int width = 256, height = 256, channels = 3;
    unsigned char img[width * height * channels];
    for (int j = height-1; j >= 0; --j) { // bottom to top
        for (int i = 0; i < width; ++i) { // left to right
            auto r = double(i)/(width-1);
            auto g = double(j)/(height-1); // 0 to 1
            auto b = 0.25;
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g); // 0 to 255
            int ib = static_cast<int>(255.999 * b);
            img[(j * width + i) * channels + 0] = ir;
            img[(j * width + i) * channels + 1] = ig;
            img[(j * width + i) * channels + 2] = ib;
        }
    }
    stbi_write_png("image.png", width, height, channels, img, width * channels);
}