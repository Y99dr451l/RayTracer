#include <iostream>
#include <fstream>
#include <random>
#include <memory>
#include <chrono>
#include <thread>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "utils.h"
#include "ray.h"
#include "vec3.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "texture.h"
#include "scenes.h"

constexpr int image_width = 1000, image_height = 1000, channels = 3, samples_per_pixel = 1000, max_depth = 50;
constexpr double aspect_ratio = double(image_width)/image_height;
unsigned char img[image_width*image_height*channels];
bool invisible_sources = true;

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0) return color(0);
    if (!world.hit(r, 0.001, infinity, rec)) return background;
    ray scattered;
    color attenuation;
    color emitted = (depth == max_depth && invisible_sources) ? color(0) : rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)) return emitted;
    return emitted + attenuation*ray_color(scattered, background, world, depth - 1);
}

void render(int i, int n_threads, const camera& cam, const color& background, const hittable& world) {
    for (int j = image_height - 1; j >= 0; --j) for (int k = i; k < image_width; k += n_threads) {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (k + random_double())/(image_width - 1), v = (image_height - 1 - j + random_double())/(image_height - 1);
            pixel_color += ray_color(cam.get_ray(u, v), background, world, max_depth);
        }
        for (int c = 0; c < channels; ++c) img[(j*image_width + k)*channels + c] = int(255.999*sqrt(pixel_color[c]/samples_per_pixel));
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();
    color background = color(0,0,0);
    auto lookfrom = point3(278, 278, -800), lookat = point3(278, 278, 0);
    auto vfov = 40.0, aperture = 0.0;
    hittable_list objects; camera cam;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    simple_light(objects, cam, background, aspect_ratio);    
    std::cerr << "World generation: " << std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
// Render
    int n_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < n_threads; i++)
        threads.push_back(std::thread([&](int i, int n_threads, const camera& cam, const color& background, const hittable& objects)
        {render(i, n_threads, cam, background, objects);}, i, n_threads, cam, background, objects));
    for (auto& t : threads) t.join();
    std::cerr << "Total time: " << std::chrono::duration <double, std::milli> (std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
    stbi_write_png("image.png", image_width, image_height, channels, img, image_width*channels);
    return 0;
}