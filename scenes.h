#ifndef SCENES_H
#define SCENES_H

#include "utils.h"
#include "vec3.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "texture.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "box.h"
#include "aarect.h"
#include "bvh.h"

#include "constant_medium.h"

void random_scene(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0.70, 0.80, 1.00);
    auto lookfrom = point3(13,2,3), lookat = point3(0,0,0);
    auto vfov = 20.0, aperture = 0.1;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    objects.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, std::make_shared<lambertian>(checker)));
    for (int a = -11; a < 11; a++) for (int b = -11; b < 11; b++) {
        point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
        if ((center - point3(4, 0.2, 0)).length() > 0.9) {
            std::shared_ptr<material> sphere_material;
            auto choose_mat = random_double();
            if (choose_mat < 0.8) { // diffuse
                auto albedo = color::random()*color::random();
                sphere_material = std::make_shared<lambertian>(albedo);
                // auto center2 = center + vec3(0, random_double(0,.5), 0);
                objects.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                // world.add(std::make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material)); // moving sphere
            } else if (choose_mat < 0.95) { // metal
                auto albedo = color::random(0.5, 1);
                auto fuzz = random_double(0, 0.5);
                sphere_material = std::make_shared<metal>(albedo, fuzz);
                objects.add(std::make_shared<sphere>(center, 0.2, sphere_material));
            } else { // glass
                sphere_material = std::make_shared<dielectric>(1.5);
                objects.add(std::make_shared<sphere>(center, 0.2, sphere_material));
            }
        }
    }
    objects.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, std::make_shared<dielectric>(1.5)));
    objects.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, std::make_shared<lambertian>(color(0.4, 0.2, 0.1))));
    objects.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0)));
}

void two_spheres(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0.70, 0.80, 1.00);
    auto lookfrom = point3(13,2,3), lookat = point3(0,0,0);
    auto vfov = 20.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    objects.add(std::make_shared<sphere>(point3(0,-10, 0), 10, std::make_shared<lambertian>(checker)));
    objects.add(std::make_shared<sphere>(point3(0, 10, 0), 10, std::make_shared<lambertian>(checker)));
}

void two_perlin_spheres(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0.70, 0.80, 1.00);
    auto lookfrom = point3(13,2,3), lookat = point3(0,0,0);
    auto vfov = 20.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto pertext = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(point3(0, 2, 0), 2, std::make_shared<lambertian>(pertext)));
}

void earth(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0.70, 0.80, 1.00);
    auto lookfrom = point3(13,2,3), lookat = point3(0,-2.5,0);
    auto vfov = 40.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto earth_texture = std::make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = std::make_shared<lambertian>(earth_texture);
    objects.add(std::make_shared<sphere>(point3(0,0,0), 2, earth_surface));
    objects.add(std::make_shared<sphere>(point3(0,-1002.5,0), 1000, std::make_shared<metal>(color(0.1), 0.0)));
}

void simple_light(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0,0,0);
    auto lookfrom = point3(26,3,6), lookat = point3(0,2,0);
    auto vfov = 20.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto pertext = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(point3(0,2,0), 2, std::make_shared<lambertian>(pertext)));
    // objects.add(std::make_shared<xy_rect>(3, 5, 1, 3, -2, std::make_shared<diffuse_light>(color(4))));
    objects.add(std::make_shared<sphere>(point3(4,3,-2), 0.3, std::make_shared<diffuse_light>(color(10))));
}

hittable_list cornell_box(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0,0,0);
    auto lookfrom = point3(278, 278, -800), lookat = point3(278, 278, 0);
    auto vfov = 40.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto red = std::make_shared<lambertian>(color(.65, .05, .05));
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    auto green = std::make_shared<lambertian>(color(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(color(15));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<flip_face>(std::make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    std::shared_ptr<material> aluminium = std::make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminium);
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);
    //std::shared_ptr<hittable> box2 = std::make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    //box2 = std::make_shared<rotate_y>(box2, -18);
    //box2 = std::make_shared<translate>(box2, vec3(130,0,65));
    //objects.add(box2);
    auto glass = std::make_shared<dielectric>(1.5);
    objects.add(std::make_shared<sphere>(point3(190, 90, 190), 90, glass));
    hittable_list lights;
    lights.add(std::make_shared<xz_rect>(213, 343, 227, 332, 554, std::shared_ptr<material>()));
    return lights;
}

hittable_list cornell_smoke(hittable_list &objects, camera &cam, color& background, double aspect_ratio) {
    background = color(0,0,0);
    auto lookfrom = point3(278, 278, -800), lookat = point3(278, 278, 0);
    auto vfov = 40.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    auto red = std::make_shared<lambertian>(color(.65, .05, .05));
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    auto green = std::make_shared<lambertian>(color(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(color(7));
    //objects.add(std::make_shared<flip_face>(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light)));
    objects.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, vec3(265,0,295));
    std::shared_ptr<hittable> box2 = std::make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, vec3(130,0,65));
    objects.add(std::make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    objects.add(std::make_shared<constant_medium>(box2, 0.01, color(1,1,1)));
    hittable_list lights;
    lights.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, std::shared_ptr<material>()));
    return lights;
}

hittable_list final_scene(hittable_list &objects, camera& cam, color& background, double aspect_ratio) {
    background = color(0,0,0);
    auto lookfrom = point3(478, 278, -600), lookat = point3(278, 278, 0);
    auto vfov = 40.0, aperture = 0.0;
    cam = camera(lookfrom, lookat, vec3(0,1,0), vfov, aspect_ratio, aperture, (lookfrom-lookat).length(), 0.0, 1.0);
    hittable_list boxes1, boxes2;
    auto ground = std::make_shared<lambertian>(color(0.48, 0.83, 0.53));
    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) for (int j = 0; j < boxes_per_side; j++) {
        auto w = 100.0;
        auto x0 = -1000.0 + i*w, z0 = -1000.0 + j*w, y0 = 0.0;
        auto x1 = x0 + w, y1 = random_double(1,101), z1 = z0 + w;
        boxes1.add(std::make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
    }
    objects.add(std::make_shared<bvh_node>(boxes1, 0, 1));
    auto light = std::make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(std::make_shared<xz_rect>(123, 423, 147, 412, 554, light));
    auto center1 = point3(400, 400, 200), center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = std::make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(std::make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));
    objects.add(std::make_shared<sphere>(point3(260, 150, 45), 50, std::make_shared<dielectric>(1.5)));
    objects.add(std::make_shared<sphere>(point3(0, 150, 145), 50, std::make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));
    auto boundary = std::make_shared<sphere>(point3(360,150,145), 70, std::make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(std::make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<sphere>(point3(0, 0, 0), 5000, std::make_shared<dielectric>(1.5));
    objects.add(std::make_shared<constant_medium>(boundary, .0001, color(1,1,1)));
    auto emat = std::make_shared<lambertian>(std::make_shared<image_texture>("earthmap.jpg"));
    objects.add(std::make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = std::make_shared<noise_texture>(0.1);
    objects.add(std::make_shared<sphere>(point3(220,280,300), 80, std::make_shared<lambertian>(pertext)));
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) boxes2.add(std::make_shared<sphere>(point3::random(0,165), 10, white));
    objects.add(std::make_shared<translate>(std::make_shared<rotate_y>(std::make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),vec3(-100,270,395)));
    hittable_list lights;
    lights.add(std::make_shared<xz_rect>(123, 423, 147, 412, 554, std::shared_ptr<material>()));
    lights.add(std::make_shared<sphere>(point3(260, 150, 45), 50, std::shared_ptr<material>()));
    lights.add(std::make_shared<sphere>(point3(360, 150, 145), 70, std::shared_ptr<material>()));
    lights.add(std::make_shared<sphere>(point3(0, 0, 0), 5000, std::shared_ptr<material>()));
    return lights;
}

#endif