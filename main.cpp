#include <iostream>
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "ray.h"
#include "vec3.h"

#define PNGCONVERSION

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    // Image
    constexpr auto aspect_ratio = 16.0/10.0;
    constexpr int image_width = 400;
    constexpr int image_height = static_cast<int>(image_width/aspect_ratio);
    constexpr int channels = 3;
    unsigned char img[image_width * image_height * channels];
    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;
    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
#ifndef PNGCONVERSION
    std::ofstream ofs;
    ofs.open("image.ppm", std::ios::out | std::ios::trunc);
    ofs << "P3\n" << image_width << ' ' << image_height << "\n255\n";
#endif
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
             auto u = double(i)/(image_width - 1);
            auto v = double(j)/(image_height - 1);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            color pixel_color = ray_color(r);
#ifdef PNGCONVERSION
            img[(j*image_width + i)*channels + 0] = static_cast<int>(255.999*pixel_color.x());
            img[(j*image_width + i)*channels + 1] = static_cast<int>(255.999*pixel_color.y());
            img[(j*image_width + i)*channels + 2] = static_cast<int>(255.999*pixel_color.z());
        }
    }
    stbi_write_png("image.png", image_width, image_height, channels, img, image_width*channels);
#else
            ofs << static_cast<int>(255.999 * pixel_color.x()) << ' '
                << static_cast<int>(255.999 * pixel_color.y()) << ' '
                << static_cast<int>(255.999 * pixel_color.z()) << '\n';
        }
    }
#endif
    std::cerr << "\nDone.\n";
}