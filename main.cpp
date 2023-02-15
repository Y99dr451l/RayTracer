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
#include "pdf.h"
#include "scenes.h"

constexpr int image_width = 400, image_height = 400, channels = 3, samples_per_pixel = 10, max_depth = 50;
constexpr double aspect_ratio = double(image_width)/image_height;
unsigned char img[image_width*image_height*channels];

color ray_color(const ray& r, const color& background, const hittable& world, std::shared_ptr<hittable> lights, int depth) {
    hit_record rec;
    if (depth <= 0) return color(0, 0, 0);
    if (!world.hit(r, 0.001, infinity, rec)) return background;
    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, srec)) return emitted;
    if (srec.is_specular) return srec.attenuation*ray_color(srec.specular_ray, background, world, lights, depth - 1);
    //std::cout << srec.is_specular << " --- " << srec.pdf_ptr << std::endl;
    auto light_ptr = std::make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);
    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());
    return emitted + srec.attenuation*rec.mat_ptr->scattering_pdf(r, rec, scattered)*ray_color(scattered, background, world, lights, depth - 1)/pdf_val;
}

void render(int i, int n_threads, const camera& cam, const color& background, const hittable& world, std::shared_ptr<hittable_list>& lights) {
    for (int j = image_height - 1; j >= 0; --j) for (int k = i; k < image_width; k += n_threads) {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (k + random_double())/(image_width - 1), v = (image_height - 1 - j + random_double())/(image_height - 1);
            pixel_color += ray_color(cam.get_ray(u, v), background, world, lights, max_depth);
        }
        for (int c = 0; c < channels; ++c) {
            if (pixel_color[c] != pixel_color[c]) pixel_color[c] = 0.0;
            img[(j*image_width + k)*channels + c] = int(255.999*sqrt(pixel_color[c]/samples_per_pixel));
        }
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();
    color background = color(0);
    hittable_list objects; camera cam;
    hittable_list lights = final_scene(objects, cam, background, aspect_ratio); // scene
    auto lights_ptr = std::make_shared<hittable_list>(lights);
    std::cerr << "World generation: " << std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
    int n_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < n_threads; i++)
        threads.push_back(std::thread([&](int i, int n_threads, const camera& cam, const color& background, const hittable& objects, std::shared_ptr<hittable_list> lights_ptr)
        {render(i, n_threads, cam, background, objects, lights_ptr);}, i, n_threads, cam, background, objects, lights_ptr));
    for (auto& t : threads) t.join();
    std::cerr << "Total time: " << std::chrono::duration <double, std::milli> (std::chrono::steady_clock::now() - start).count() << " ms" << std::endl;
    stbi_write_png("image.png", image_width, image_height, channels, img, image_width*channels);
    return 0;
}