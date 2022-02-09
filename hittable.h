#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray_tracer.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<material> material;

    inline void setFrontFace(const ray& ray, const vec3& outwardNormal) {
        front_face = dot(ray.direction(), outwardNormal) < 0;
        if (front_face) {
            normal = outwardNormal;
        }else{
            normal = -outwardNormal;
        }
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif
