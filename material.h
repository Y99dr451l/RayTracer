#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "pdf.h"
#include "texture.h"

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    color attenuation;
    std::shared_ptr<pdf> pdf_ptr;
};

struct material {
    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const {return color(0, 0, 0);}
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {return false;}
    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {return 0;}
};

struct lambertian : public material {
    lambertian(const color& a) : albedo(std::make_shared<solid_color>(a)) {}
    lambertian(std::shared_ptr<texture> a) : albedo(a) {}
    std::shared_ptr<texture> albedo;
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
        return true;
    }
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine/Pi;
    }
};

struct metal : public material {
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    color albedo;
    double fuzz;
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        return true;
    }
};

struct dielectric : public material {
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}
    double ir;
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
        bool cannot_refract = refraction_ratio*sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) direction = reflect(unit_direction, rec.normal);
        else direction = refract(unit_direction, rec.normal, refraction_ratio);
        srec.specular_ray = ray(rec.p, direction, r_in.time());
        return true;
    }
    static double reflectance(double cosine, double ref_idx) {
        auto r0 = (1 - ref_idx)/(1 + ref_idx); r0 = r0*r0;
        return r0 + (1 - r0)*pow((1 - cosine), 5);
    }
};

struct diffuse_light : public material {
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}
    std::shared_ptr<texture> emit;
    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override {
        //if (!rec.front_face) return color(0, 0, 0);
        return emit->value(u, v, p);
    }
};

struct isotropic : public material {
    isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<texture> a) : albedo(a) {}
    std::shared_ptr<texture> albedo;
#if 0
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
#endif
};

#endif