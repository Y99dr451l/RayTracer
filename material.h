#ifndef MATERIAL_H
#define MATERIAL_H
#include "utils.h"
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "texture.h"
#include "hit_record.h"

struct material {
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
    virtual color emitted(double u, double v, const point3& p) const {return color(0,0,0);}
};

struct lambertian : public material {
    lambertian(const color& a) : albedo(std::make_shared<solid_color>(a)) {}
    lambertian(std::shared_ptr<texture> a) : albedo(a) {}
    std::shared_ptr<texture> albedo;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()) scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }      
};

struct metal : public material {
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    color albedo; double fuzz;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

struct dielectric : public material {
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}
    double ir;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0), sin_theta = sqrt(1.0 - cos_theta*cos_theta);
        bool cannot_refract = refraction_ratio*sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) direction = reflect(unit_direction, rec.normal);
        else direction = refract(unit_direction, rec.normal, refraction_ratio);
        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }
    static double reflectance(double cosine, double ref_idx) { // Schlick approximation
        auto r0 = pow((1 - ref_idx)/(1 + ref_idx), 2);
        return r0 + (1 - r0)*pow((1 - cosine), 5);
    }
};

struct diffuse_light : public material  {
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {return false;}
    virtual color emitted(double u, double v, const point3& p) const override {return emit->value(u, v, p);}
    std::shared_ptr<texture> emit;
};

struct isotropic : public material {
    isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<texture> a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
    std::shared_ptr<texture> albedo;
};

#endif