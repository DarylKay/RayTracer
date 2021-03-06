#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray_tracer.h"
#include "aabb.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    double u,v;
    bool front_face;
    shared_ptr<material> material;

    inline void setFaceNormal(const ray& ray, const vec3& outwardNormal) {
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
    virtual bool boundBox(double t_min, double t_max, aabb& box) const = 0;
};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> obj, const vec3& _offset) : moved(obj), offset(_offset) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool boundBox(double t_min, double t_max, aabb& box) const override;

    shared_ptr<hittable> moved;
    vec3 offset;
};

bool translate::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!moved->hit(moved_r, t_min, t_max, rec)){
        return false;
    }

    rec.p += offset;
    rec.setFaceNormal(moved_r, rec.normal);

    return true;
}

bool translate::boundBox(double t_min, double t_max, aabb &box) const {
    if(!moved->boundBox(t_min, t_max, box)){
        return false;
    }

    box = aabb(box.minimum() + offset, box.maximum() + offset);

    return true;
}

class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool boundBox(double time0, double time1, aabb& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
};

rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
    auto radians = degToRad(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->boundBox(0, 1, bbox);

    point3 min( infinity,  infinity,  infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bbox.maximum().x() + (1-i)*bbox.minimum().x();
                auto y = j*bbox.maximum().y() + (1-j)*bbox.minimum().y();
                auto z = k*bbox.maximum().z() + (1-k)*bbox.minimum().z();

                auto newx =  cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;

    p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

    rec.p = p;
    rec.setFaceNormal(rotated_r, normal);

    return true;
}

#endif
