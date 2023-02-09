#ifndef HITTABLE_H
#define HITTABLE_H
#include <memory>
#include <vector>
#include "ray.h"
#include "vec3.h"
#include "material.h"

struct material;

struct hit_record {
    point3 p; vec3 normal;
    double t; bool front_face;
    std::shared_ptr<material> mat_ptr;
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = (dot(r.direction(), outward_normal) < 0) ? outward_normal : -outward_normal;
    }
};

struct hittable {
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

struct hittable_list : public hittable {
    hittable_list() {}
    hittable_list(std::shared_ptr<hittable> object) {add(object);}
    std::vector<std::shared_ptr<hittable>> objects;
    void clear() {objects.clear();}
    void add(std::shared_ptr<hittable> object) {objects.push_back(object);}
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        hit_record temp_rec; bool hit_anything = false; auto closest_so_far = t_max;
        for (const auto& object : objects) {
            if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }
};

struct sphere : public hittable {
    sphere() {}
    sphere(point3 cen, double r, std::shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
    point3 center; double radius;
    std::shared_ptr<material> mat_ptr;
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 oc = r.origin() - center; // (A + tB - C).(A + tB - C) = r^2 => B^2t^2 + 2ABt + A^2 - r^2 = 0
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius; 
        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant), root = (-half_b - sqrtd)/a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd)/a;
            if (root < t_min || t_max < root) return false;
        }
        rec.t = root; rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center)/radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }
};
#endif