#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"

class sphere : public hittable {
    public:
        sphere();
        sphere(point3 cen, double rad, shared_ptr<material> mat) : center(cen), radius(rad), material(mat){};
        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        bool boundBox(double t_min, double t_max, aabb& box) const override;


        point3 center;
        double radius;
        shared_ptr<material> material;
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
    vec3 outwardNormal = unitVector(rec.p - center); //unit vector
    rec.setFrontFace(r, outwardNormal);
    rec.material = material;

    return true;
}

bool sphere::boundBox(double t_min, double t_max, aabb& box) const {
    box = aabb(center - vec3(radius,radius,radius), center + vec3(radius,radius,radius));
    return true;
}

#endif
