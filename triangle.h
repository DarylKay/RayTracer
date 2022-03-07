#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"

class triangle : public hittable {
    public:
        triangle();
        triangle(point3 p1, point3 p2, point3 p3, bool cw, shared_ptr<material> mat) : a(p1), b(p2), c(p3), clockWise(cw), material(mat){};

        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        bool boundBox(double t_min, double t_max, aabb& box) const override;

        point3 a;
        point3 b;
        point3 c;

        bool clockWise;

        shared_ptr<material> material;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 normal = unitVector(cross(b - a, c - a));

    if (clockWise) {
        normal = -normal;
    }

    double dirDotNormal = dot(r.direction(),normal);

    if (fabs(dirDotNormal) < epsilon) { //parallel check
        return false;
    }

    double t = dot((a - r.origin()), normal) / dirDotNormal;

    if (t < t_min || t > t_max) {
        return false;
    }

    point3 P = r.at(t);

    vec3 na = unitVector(cross(c - b, P - b));
    double alpha = dot(normal,na) / dot(normal, normal);
    if (alpha < 0) {
        return false;
    }

    vec3 nb = unitVector(cross(a - c, P - c));
    double beta = dot(normal,nb) / dot(normal,normal);
    if (beta < 0) {
        return false;
    }

    vec3 nc = unitVector(cross(b - a, P - a));
    double gamma = dot(normal,nc) / dot(normal,normal);
    if (gamma < 0) {
        return false;
    }

    rec.t = t;
    rec.p = P;
    vec3 outwardNormal = normal; //unit vector
    rec.setFrontFace(r, outwardNormal);
    rec.material = material;

    return true;
}

bool triangle::boundBox(double t_min, double t_max, aabb& box) const {
    point3 min = minVals(a, minVals(b,c));
    point3 max = maxVals(a, maxVals(b,c));

    box = aabb(min, max);

    return true;
}

#endif //TRIANGLE_H
