#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "vec3.h"
#include "ray.h"
#include "material.h"

struct material;

struct hit_record {
    point3 p; vec3 normal;
    double t, u, v; bool front_face;
    std::shared_ptr<material> mat_ptr;
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = (dot(r.direction(), outward_normal) < 0) ? outward_normal : -outward_normal;
    }
};

#endif