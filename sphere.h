#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"

struct sphere : public hittable {
    sphere() {}
    sphere(point3 cen, double r, std::shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
    point3 center; double radius;
    std::shared_ptr<material> mat_ptr;
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 oc = r.origin() - center; // (A + tB - C).(A + tB - C) = r^2 => B^2t^2 + 2ABt + A^2 - r^2 = 0
        auto a = r.direction().length_squared(), half_b = dot(oc, r.direction()), c = oc.length_squared() - radius*radius; 
        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant), root = (-half_b - sqrtd)/a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd)/a;
            if (root < t_min || t_max < root) return false;
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        auto outward_normal = (rec.p - center)/radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat_ptr = mat_ptr;
        return true;
    }
    bool bounding_box(double time0, double time1, aabb& output_box) const {
        output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
        return true;
    }
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        auto theta = acos(-p.y()), phi = atan2(-p.z(), p.x()) + Pi;
        u = phi/(2*Pi); v = theta/Pi;
    }
    double pdf_value(const point3& o, const vec3& v) const {
        hit_record rec;
        if (!this->hit(ray(o, v), 0.001, infinity, rec)) return 0;
        auto cos_theta_max = sqrt(1 - radius*radius/(center - o).length_squared());
        auto solid_angle = 2*Pi*(1 - cos_theta_max);
        return  1/solid_angle;
    }
    vec3 random(const point3& o) const {
        vec3 direction = center - o;
        auto distance_squared = direction.length_squared();
        onb uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }
};

#endif