#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : hittable {
    public:
        sphere();
        sphere(point3 cen, double rad) : center(cen), radius(rad) {};
        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        point3 center;
        double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
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

    return true;
}

#endif
