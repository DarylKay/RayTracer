#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray_tracer.h"

struct hit_record;

class material {
public:
    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

class metal : public material {
public:
    metal(const color& a, double f) : metColor(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 direction = reflect(unitVector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, direction + fuzz * randomUnitSphere());
        attenuation = metColor;
        return (dot(direction, rec.normal) > 0);
    }

    color metColor;
    double fuzz;
};

class lambertian : public material {
public:
    lambertian(const color& a) : lambColor(a) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override{
        vec3 direction = lambertianScatter(rec.normal);

        if (direction.near_zero()) {
            direction = rec.normal;
        }

        scattered = ray(rec.p, direction);

        attenuation = lambColor;
        return true;
    }

    color lambColor;
};

class dielectric : public material {
public :
    dielectric (const double ind) : indice(ind) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        double ratio = rec.front_face ? (1.0/indice) : indice;
        vec3 direction = refract(r_in.direction(), rec.normal, ratio);

        attenuation = color(1.0,1.0,1.0);
        scattered = ray(rec.p, direction);
        return true;
    }

    double indice;
};
#endif //MATERIAL_H
