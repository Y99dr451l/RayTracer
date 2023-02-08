#ifndef CAMERA_H
#define CAMERA_H
#include "utils.h"
#include "ray.h"
#include "vec3.h"

struct camera {
        camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspect_ratio, double aperture, double focus_dist) {
            auto theta = Pi*vfov/180.;
            auto h = tan(theta/2);
            auto viewport_height = 2.0*h, viewport_width = aspect_ratio*viewport_height;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            origin = lookfrom;
            horizontal = focus_dist*viewport_width*u; vertical = focus_dist*viewport_height*v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;
            lens_radius = aperture / 2;
        }
        point3 origin, lower_left_corner;
        vec3 horizontal, vertical, u, v, w;
        double lens_radius;
        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius*random_in_unit_disk(), offset = u*rd.x() + v*rd.y();
            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
        }        
};

#endif