#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "ray_tracer.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:
    constant_medium(shared_ptr<hittable> bounds, double density, shared_ptr<texture> text) : boundary(bounds), neg_inverted_density(-1/density), isotropicFunction(make_shared<isotropic>(text)){}
    constant_medium(shared_ptr<hittable> bounds, double density, RGB c) : boundary(bounds), neg_inverted_density(-1/density), isotropicFunction(make_shared<isotropic>(c)){}

    virtual bool boundBox(double time0, double time1, aabb& outBox) const override {
        return boundary->boundBox(time0,time1,outBox);
    }

    virtual bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const override;

    shared_ptr<material> isotropicFunction;
    shared_ptr<hittable> boundary;
    double neg_inverted_density;
};

bool constant_medium::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    hit_record rec1, rec2;

    if (!boundary->hit(r,-infinity, infinity, rec1)){
        return false;
    }

    if (!boundary->hit(r, rec1.t+epsilon, infinity, rec2)) {
        return false;
    }

    if(rec1.t < t_min) rec1.t = t_min;
    if(rec2.t > t_max) rec2.t = t_max;

    if (rec1.t >= rec2.t) {
        return false;
    }

    if (rec1.t < 0) {
        rec1.t = 0;
    }

    const double ray_length = r.direction().length();
    const double distance_in_boundary = (rec2.t - rec1.t) * ray_length;
    const double hit_distance = neg_inverted_density * log(randomDouble());
    //function for getting the invertet_density for a gradual medium approach

    if (hit_distance > distance_in_boundary) {
        return false;
    }

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = vec3(1,0,0);
    rec.front_face = true;
    rec.material = isotropicFunction;

    return true;
}

#endif //CONSTANT_MEDIUM_H
