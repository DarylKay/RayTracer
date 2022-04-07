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

class rotateY : public hittable {
public:
    rotateY(shared_ptr<hittable> obj, double angle);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool boundBox(double t_min, double t_max, aabb& box) const override {
        box = bBox;
        return hasBox;
    }

    shared_ptr<hittable> object;
    double sinTheta;
    double cosTheta;
    bool hasBox;
    aabb bBox;
};

rotateY::rotateY(shared_ptr<hittable> obj, double angle) : object(obj) {
    double radians = degToRad(angle);

    sinTheta = sin(radians);
    cosTheta = cos(radians);

    hasBox = object->boundBox(0,1,bBox);

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bBox.maximum().x() + (1-i)*bBox.minimum().x();
                auto y = j*bBox.maximum().y() + (1-j)*bBox.minimum().y();
                auto z = k*bBox.maximum().z() + (1-k)*bBox.minimum().z();

                auto newx =  cosTheta*x + sinTheta*z;
                auto newz = -sinTheta*x + cosTheta*z;


                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bBox = aabb(min, max);
}

bool rotateY::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    point3 origin = r.origin();
    point3 direction = r.direction();

    origin[0] = cosTheta*r.origin()[0] - sinTheta*r.origin()[2];
    origin[2] = sinTheta*r.origin()[0] + cosTheta*r.origin()[2];

    direction[0] = cosTheta*r.origin()[0] - sinTheta*r.origin()[2];
    direction[2] = sinTheta*r.origin()[0] + cosTheta*r.origin()[2];

    ray rotatedR(origin, direction, r.time());

    if(!object->hit(rotatedR, t_min, t_max, rec)){
        return false;
    }

    point3 p = rec.p;
    vec3 normal = rec.normal;

    p[0] =  cosTheta*rec.p[0] + sinTheta*rec.p[2];
    p[2] = -sinTheta*rec.p[0] + cosTheta*rec.p[2];

    normal[0] =  cosTheta*rec.normal[0] + sinTheta*rec.normal[2];
    normal[2] = -sinTheta*rec.normal[0] + cosTheta*rec.normal[2];

    rec.p = p;
    rec.setFaceNormal(rotatedR, normal);

    return true;
}

#endif
