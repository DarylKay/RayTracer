#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"

class triangle : public hittable {
    public:
        triangle() {};
        triangle(point3 p1, point3 p2, point3 p3, bool cw, shared_ptr<material> mat, double u[], double v[], bool useGivenNorm, vec3 n[]) : a(p1), b(p2), c(p3), clockWise(cw), material(mat){
          uvU[0] = u[0];
          uvU[1] = u[1];
          uvU[2] = u[2];

          uvV[0] = v[0];
          uvV[1] = v[1];
          uvV[2] = v[2];

          if (useGivenNorm) {
              normals[0] = n[0];
              normals[1] = n[1];
              normals[2] = n[2];
          }

          calculateNormal = !useGivenNorm;
        };

        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        bool boundBox(double t_min, double t_max, aabb& box) const override;

        point3 a;
        point3 b;
        point3 c;

        double uvU[3];
        double uvV[3];

        vec3 normals[3];
        bool calculateNormal;

        bool clockWise;

        shared_ptr<material> material;

        void getTriangleUV(double &u, double &v, double ba, double bb, double bc) const {
            u = (ba * uvU[0] + bb * uvU[1] + bc * uvU[2]);
            v = (ba * uvV[0] + bb * uvV[1] + bc * uvV[2]);
        }
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 normal = (cross(b - a, c - a));

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

    double recipDot = 1 / dot (normal, normal);

    vec3 na = (cross(c - b, P - b));
    double baryA = dot(normal,na) * recipDot;
    if (baryA < 0) {
        return false;
    }

    vec3 nb = (cross(a - c, P - c));
    double baryB = dot(normal,nb) * recipDot;
    if (baryB < 0) {
        return false;
    }

    vec3 nc = (cross(b - a, P - a));
    double baryC = dot(normal,nc) * recipDot;
    if (baryC < 0) {
        return false;
    }

    if (baryA + baryB + baryC > 1 + epsilon){
        return false;
    }

    rec.t = t;
    rec.p = P;
    //vec3 outwardNormal = unitVector(normal); //unit vector rigid triangles
    vec3 outwardNormal;
    if (calculateNormal) {
        outwardNormal = unitVector(na * baryA + nb * baryB + nc * baryC); //interpolated normals with barrycentric coordinates
    } else {
        outwardNormal = unitVector(normals[0] * baryA + normals[1] * baryB + normals[2] * baryC); //interpolated normals with barrycentric coordinates
    }
    rec.setFaceNormal(r, outwardNormal);
    getTriangleUV(rec.u, rec.v, baryA, baryB, baryC);
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
