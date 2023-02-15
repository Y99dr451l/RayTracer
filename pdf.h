#ifndef PDF_H
#define PDF_H

#include <memory>
#include "vec3.h"
#include "onb.h"

struct pdf {
    virtual ~pdf() {}
    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

struct cosine_pdf : public pdf {
    cosine_pdf(const vec3& w) {uvw.build_from_w(w);}
    onb uvw;
    virtual double value(const vec3& direction) const override {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine/Pi;
    }
    virtual vec3 generate() const override {return uvw.local(random_cosine_direction());}
};

struct hittable_pdf : public pdf {
    hittable_pdf(std::shared_ptr<hittable> p, const point3& origin) : ptr(p), o(origin) {}
    point3 o; std::shared_ptr<hittable> ptr;
    virtual double value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }
    virtual vec3 generate() const override {return ptr->random(o);}
};

struct mixture_pdf : public pdf {
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {p[0] = p0; p[1] = p1;}
    std::shared_ptr<pdf> p[2];
    virtual double value(const vec3& direction) const override {
        return 0.5*p[0]->value(direction) + 0.5*p[1]->value(direction);
    }
    virtual vec3 generate() const override {
        if (random_double() < 0.5) return p[0]->generate();
        else return p[1]->generate();
    }
};

#endif