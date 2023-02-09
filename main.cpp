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
#include "material.h"

hittable_list random_scene() {
    hittable_list world;
    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
    for (int a = -11; a < 11; a++) for (int b = -11; b < 11; b++) {
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;
                auto choose_mat = random_double();
                if (choose_mat < 0.8) sphere_material = std::make_shared<lambertian>(color::random()*color::random());
                else if (choose_mat < 0.95) sphere_material = std::make_shared<metal>(color::random(0.5, 1), random_double(0, 0.5));
                else sphere_material = std::make_shared<dielectric>(1.5);
                world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
            }
    }
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, std::make_shared<dielectric>(1.5)));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, std::make_shared<lambertian>(color(0.4, 0.2, 0.1))));
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0)));
    return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0) return color(0,0,0);
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered; color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) return attenuation*ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    auto t = 0.5*(unit_vector(r.direction()).y() + 1.0);
    return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

constexpr int image_width = 1000, image_height = 625, channels = 3, samples_per_pixel = 100, max_depth = 50;
constexpr double aspect_ratio = double(image_width)/image_height;
unsigned char img[image_width*image_height*channels];

void render(int i, int n_threads, const camera& cam, const hittable& world) {
    for (int j = image_height - 1; j >= 0; --j) for (int k = i; k < image_width; k += n_threads) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (k + random_double())/(image_width - 1), v = (image_height - 1 - j + random_double())/(image_height - 1);
                pixel_color += ray_color(cam.get_ray(u, v), world, max_depth);
            }
            for (int c = 0; c < channels; ++c) img[(j*image_width + k)*channels + c] = int(255.999*sqrt(pixel_color[c]/samples_per_pixel));
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();
    hittable_list world = random_scene();
    std::cerr << "World generation: " << std::chrono::duration <double, std::milli> (std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
// Camera
    point3 lookfrom(13,2,10), lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 15.0, aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
// Render
    int n_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < n_threads; i++)
        threads.push_back(std::thread([&](int i, int n_threads, const camera& cam, const hittable& world) {render(i, n_threads, cam, world);}, i, n_threads, cam, world));
    for (auto& t : threads) t.join();
    auto end = std::chrono::steady_clock::now();
    std::cerr << "Total time: " << std::chrono::duration <double, std::milli> (std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
    stbi_write_png("image.png", image_width, image_height, channels, img, image_width*channels);
    return 0;
}