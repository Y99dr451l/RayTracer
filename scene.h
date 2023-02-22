#ifndef SCENE_H
#define SCENE_H

#include <thread>
#include <mutex>
#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

class scene {
  public:
    void render() {
        const int image_height = static_cast<int>(image_width/aspect_ratio);
        unsigned char* img = new unsigned char[image_width * image_height * 3];
        cam.initialize(aspect_ratio);
        int sqrt_spp = int(sqrt(samples_per_pixel));
        int n_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        std::mutex cout_mutex;
        for (int k = 0; k < n_threads; k++) {
            threads.push_back(std::thread([&, k]() {
                cout_mutex.lock();
                std::cout << "Thread " << k << " started" << std::endl;
                cout_mutex.unlock();
                for (int j = 0; j < image_height; ++j) {
                    for (int i = k; i < image_width; i += n_threads) {
                        color pixel_color(0, 0, 0);
                        for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                            for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                                auto s = (i + (s_i + random_double()) / sqrt_spp) / (image_width - 1);
                                auto t = (j + (s_j + random_double()) / sqrt_spp) / (image_height - 1);
                                ray r = cam.get_ray(s, t);
                                pixel_color += ray_color(r, max_depth);
                            }
                        }
                        // write_color(std::cout, pixel_color, samples_per_pixel);
                        for (int c = 0; c < 3; ++c) {
                            if (pixel_color[c] != pixel_color[c]) pixel_color[c] = 0.0;
                            img[(j * image_width + i) * 3 + c] = int(255.999 * sqrt(pixel_color[c] / samples_per_pixel));
                        }
                    }
                }
                std::cout << "Thread " << k << " ended" << std::endl;
            }));
        }
        for (auto& t : threads) t.join();
        stbi_write_png("image.png", image_width, image_height, 3, img, image_width * 3);
    }

  public:
    hittable_list world;
    hittable_list lights;
    camera cam;

    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 100;
    int max_depth = 20;
    color background = color(0,0,0);

  private:
    color ray_color(const ray& r, int depth) {
        hit_record rec;
        if (depth <= 0) return color(0,0,0);
        if (!world.hit(r, interval(0.001, infinity), rec)) return background;
        scatter_record srec;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);
        if (!rec.mat->scatter(r, rec, srec)) return color_from_emission;
        if (srec.skip_pdf) return srec.attenuation * ray_color(srec.skip_pdf_ray, depth-1);
        auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);
        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_val = p.value(scattered.direction());
        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
        color color_from_scatter = (srec.attenuation * scattering_pdf * ray_color(scattered, depth-1)) / pdf_val;
        return color_from_emission + color_from_scatter;
    }
};

#endif
