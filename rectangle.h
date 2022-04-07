#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "ray_tracer.h"
#include "hittable.h"

class rectangle : public hittable {
public:
    rectangle(){}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool boundBox(double t_min, double t_max, aabb& box) const = 0;
};

class xyRect : public rectangle {
public:
    xyRect(){}
    xyRect(double X0, double X1, double Y0, double Y1, double Z, shared_ptr<material> mat) : x0(X0), x1(X1), y0(Y0), y1(Y1), z(Z), material(mat){
    };

    double x0, x1, y0, y1, z;
    shared_ptr<material> material;

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (z-r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max) {
            return false;
        }

        point3 P = r.at(t);
        double x = P.x();
        double y = P.y();
        //double x = r.origin().x() + t * r.direction().x();
        //double y = r.origin().y() + t * r.direction().y();

        if (x < x0 || x > x1 || y < y0 || y > y1) {
            return false;
        }

        rec.t = t;
        rec.p = P;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (y - y0) / (y1 - y0);

        vec3 outwardNormal(0,0,1);
        rec.setFaceNormal(r, outwardNormal);
        rec.material = material;

        return true;
    }

    bool boundBox(double t_min, double t_max, aabb& box) const override {
        box = aabb(point3(x0,y0,z-0.0001),point3(x1,y1,z+0.0001));
        return true;
    }
};

class yzRect : public rectangle {
public:
    yzRect(){}
    yzRect(double Y0, double Y1, double Z0, double Z1, double X, shared_ptr<material> mat) : y0(Y0), y1(Y1), z0(Z0), z1(Z1), x(X), material(mat){
    };

    double y0, y1, z0, z1, x;
    shared_ptr<material> material;

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (x-r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max) {
            return false;
        }

        point3 P = r.at(t);
        double y = P.y();
        double z = P.z();
        //double x = r.origin().x() + t * r.direction().x();
        //double y = r.origin().y() + t * r.direction().y();

        if (y < y0 || y > y1 || z < z0 || z > z1) {
            return false;
        }

        rec.t = t;
        rec.p = P;

        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);

        vec3 outwardNormal(1,0,0);
        rec.setFaceNormal(r, outwardNormal);
        rec.material = material;

        return true;
    }

    bool boundBox(double t_min, double t_max, aabb& box) const override {
        box = aabb(point3(x-0.0001,y0,z0),point3(x+0.0001,y1,z1));
        return true;
    }
};

class xzRect : public rectangle {
public:
    xzRect(){}
    xzRect(double X0, double X1, double Z0, double Z1, double Y, shared_ptr<material> mat) : x0(X0), x1(X1), z0(Z0), z1(Z1), y(Y), material(mat){
    };

    double x0, x1, z0, z1, y;
    shared_ptr<material> material;

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        double t = (y-r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max) {
            return false;
        }

        point3 P = r.at(t);
        double x = P.x();
        double z = P.z();
        //double x = r.origin().x() + t * r.direction().x();
        //double y = r.origin().y() + t * r.direction().y();

        if (x < x0 || x > x1 || z < z0 || z > z1) {
            return false;
        }

        rec.t = t;
        rec.p = P;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);

        vec3 outwardNormal(0,1,0);
        rec.setFaceNormal(r, outwardNormal);
        rec.material = material;

        return true;
    }

    bool boundBox(double t_min, double t_max, aabb& box) const override {
        box = aabb(point3(x0,y-0.0001,z0),point3(x1,y+0.0001,z1));
        return true;
    }
};

#endif //RECTANGLE_H
