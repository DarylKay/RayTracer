#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "ray_tracer.h"

struct hit_record;

class material {
public:
    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;

    virtual color emitted() const{
        return color(0,0,0);
    }

};

class emissive : public material {
public:
    emissive(const color &a) : emit(a) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted() const override {
        return emit;
    }

    color emit;
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
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a ) : albedo(a){}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override{
        vec3 direction = lambertianScatter(rec.normal);

        if (direction.near_zero()) {
            direction = rec.normal;
        }

        scattered = ray(rec.p, direction);

        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

class dielectric : public material {
public :
    dielectric (const double ind) : indice(ind) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        double ratio = rec.front_face ? (1.0/indice) : indice;

        vec3 unitDirection = r_in.direction();

        double cos_theta = fmin(dot(-unitDirection, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        vec3 direction;

        if ((ratio * sin_theta > 1.0) || (reflectance(cos_theta, ratio) > randomDouble())) {
            direction = reflect(unitDirection, rec.normal);
        } else {
            direction = refract(unitDirection, rec.normal, ratio);
        }

        attenuation = color(1.0,1.0,1.0);
        scattered = ray(rec.p, direction);
        return true;
    }

    double indice;

    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};
#endif //MATERIAL_H
