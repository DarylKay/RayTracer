#ifndef PLANE_H
#define PLANE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"


class plane : public hittable {
public:
    plane();
    plane(int z, shared_ptr<material> mat) : material(mat){

    };
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    shared_ptr<material> material;
};

bool plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double halfB = dot(oc, r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = halfB * halfB - a * c;
    if (discriminant < 0) {
        return false;
    }

    double sqrtDet = sqrt(discriminant);
    double root = (-halfB - sqrtDet) / a;

    //recheck the root with + sqrt discriminant
    if (root < t_min || root > t_max) {
        root = (-halfB + sqrtDet) / a;
        if (root < t_min || root > t_max) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(root);
    vec3 outwardNormal = (rec.p - center) / radius; //unit vector
    rec.setFrontFace(r, outwardNormal);
    rec.material = material;

    return true;
}


#endif //PLANE_H
