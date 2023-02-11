#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "vec3.h"
#include "perlin.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct texture {
    virtual color value(double u, double v, const point3& p) const = 0;
};

struct solid_color : public texture {
    solid_color() {}
    solid_color(color c) : color_value(c) {}
    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}
    color color_value;
    virtual color value(double u, double v, const vec3& p) const override {return color_value;}    
};

struct checker_texture : public texture {
    checker_texture() {}
    checker_texture(std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
    checker_texture(color c1, color c2) : even(std::make_shared<solid_color>(c1)) , odd(std::make_shared<solid_color>(c2)) {}
std::shared_ptr<texture> odd, even;
    virtual color value(double u, double v, const point3& p) const override {
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if (sines < 0) return odd->value(u, v, p);
        else return even->value(u, v, p);
    }
};

struct noise_texture : public texture {
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}
    perlin noise; double scale;
    virtual color value(double u, double v, const point3& p) const override {
        return color(1,1,1)*0.5*(1.0 + noise.noise(scale*p)); // smooth noise
        // return color(1,1,1)*noise.turb(scale*p); // direct turbulence (camo)
        // return color(1,1,1)*0.5*(1 + sin(scale*p.z() + 10*noise.turb(p))); // indirect turbulence (marble)
    }
};

struct image_texture : public texture {
    image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}
    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;
        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }
        bytes_per_scanline = bytes_per_pixel*width;
    }
    ~image_texture() {delete data;}
    const static int bytes_per_pixel = 3;
    unsigned char *data;
    int width, height, bytes_per_scanline;
    virtual color value(double u, double v, const vec3& p) const override {
        if (data == nullptr) return color(0,1,1);
        u = clamp(u, 0.0, 1.0); v = 1.0 - clamp(v, 0.0, 1.0); // flip v to image coordinates
        auto i = static_cast<int>(u*width), j = static_cast<int>(v*height);
        if (i >= width) i = width-1; if (j >= height) j = height-1;
        const auto color_scale = 1.0/255.0;
        auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }
};

#endif