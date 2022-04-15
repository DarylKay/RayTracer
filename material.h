#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "ray_tracer.h"

struct hit_record;

class material {
public:
    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const = 0;

    virtual double emitted(double lambda) const {
        return 0.0;
    }
};

class emissive : public material {
public:
    emissive(const RGB &a) : emit(SampledSpectrum::FromRGB(a,SpectrumType::Illuminant)) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override {
        return false;
    }

    virtual double emitted(double lambda) const override {
        return InterpolateSpectrumSamples(emit, lambda);
    }

    SampledSpectrum emit;
};

class metal : public material {
public:
    metal(const RGB& a, double f) : metColor(SampledSpectrum::FromRGB(a)), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override {
        vec3 direction = reflect(unitVector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, direction + fuzz * randomUnitSphere());
        attenuation = InterpolateSpectrumSamples(metColor, lambda);
        return (dot(direction, rec.normal) > 0);
    }

    SampledSpectrum metColor;
    double fuzz;
};

class glossy : public material {
public:
    glossy(const RGB& a, double r) : glossyColor(SampledSpectrum::FromRGB(a)), reflectance(r) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override {
        vec3 directionSpecular = reflect(unitVector(r_in.direction()), rec.normal);
        ray Specular = ray(rec.p, directionSpecular);

        vec3 directionDiffuse = lambertianScatter(rec.normal);

        if (directionDiffuse.near_zero()) {
            directionDiffuse = rec.normal;
        }

        ray Diffuse = ray(rec.p, directionDiffuse);

        if(randomDouble() > reflectance)
        {
            scattered = Diffuse;
            attenuation = InterpolateSpectrumSamples(glossyColor, lambda);
            return true;
        } else {
            scattered = Specular;
            attenuation = InterpolateSpectrumSamples(glossyColor, lambda);
            return (dot(directionSpecular, rec.normal) > 0);
        }
    }

    SampledSpectrum glossyColor;
    double reflectance;
};

class lambertian : public material {
public:
    lambertian(const RGB& a) : albedo(make_shared<spectrum_color>(a)) {}
    lambertian(shared_ptr<texture> a ) : albedo(a){}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override{
        vec3 direction = lambertianScatter(rec.normal);

        if (direction.near_zero()) {
            direction = rec.normal;
        }

        scattered = ray(rec.p, direction);

        attenuation = albedo->value(rec.u, rec.v, rec.p, lambda);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

class dielectric : public material {
public :
    dielectric (const double ind, const double d) : indice(ind), dispersion(d) {}

    virtual bool scatter(
            const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override {
        double cauchy = indice + (dispersion/(lambda * lambda));
        double ratio = rec.front_face ? (1.0/cauchy) : cauchy;

        vec3 unitDirection = r_in.direction();

        double cos_theta = fmin(dot(-unitDirection, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        vec3 direction;

        if ((ratio * sin_theta > 1.0) || (reflectance(cos_theta, ratio) > randomDouble())) {
            direction = reflect(unitDirection, rec.normal);
        } else {
            direction = refract(unitDirection, rec.normal, ratio);
        }

        attenuation = 1.0;
        scattered = ray(rec.p, direction);
        return true;
    }

    double indice;
    double dispersion;

    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

class isotropic : public material {
public:
    isotropic(const RGB& c) : albedo(make_shared<spectrum_color>(c)) {}
    isotropic(shared_ptr<texture> text) : albedo(text) {}

    virtual bool scatter(
        const ray& r_in, const struct hit_record& rec, double& attenuation, ray& scattered, double lambda
    ) const override {
        scattered = ray(rec.p, randomUnitSphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p, lambda);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};
#endif //MATERIAL_H
