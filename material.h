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
    metal(const color& a) : metColor(a) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 direction = reflect(unitVector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, direction);
        attenuation = metColor;
        return (dot(direction, rec.normal) > 0);
    }

    color metColor;
};

class lambertian : public material {
public:
    lambertian(const color& a) : lambColor(a) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override{
        vec3 direction = lambertianScatter(rec.normal);
        scattered = ray(rec.p, direction);

        if (direction.near_zero())
            direction = rec.normal;

        attenuation = lambColor;
        return true;
    }

    color lambColor;
};

#endif //MATERIAL_H
