#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "ray_tracer.h"
#include <string>

class background {
public:
    virtual double backgroundColor(const ray& r, double lambda) const = 0;
};

class solid_background : public background {
public:
    solid_background(){}
    solid_background(const RGB &solidColor) : backColor(SampledSpectrum::FromRGB(solidColor)){};
    solid_background(double a, double b, double c) : backColor(SampledSpectrum::FromRGB(RGB(a,b,c))){};

    virtual double backgroundColor(const ray& r, double lambda) const override {
        return InterpolateSpectrumSamples(backColor, lambda);
    }

    SampledSpectrum backColor;
};

/*
class environment_map : public background{
public:
    const int bytes_per_pixel = 3;

    environment_map() {}
    environment_map(char *folder) {
        //0:-x, 1:+x, 2:-y, 3:+y, 4:-z, 5:+z
        const string sides[] = {"/nx.jpg", "/px.jpg", "/ny.jpg", "/py.jpg", "/nz.jpg", "/pz.jpg"};

        auto components_per_pixel = bytes_per_pixel;

        for (int i = 0; i < 6; i++) {

            string ending = folder + sides[i];
            char* charArr = &ending[0];

            cerr << "\rLoading image: " << charArr << " " << std::flush;

            data[i] = stbi_load(
                    charArr, &width, &height, &components_per_pixel, components_per_pixel);

            if (!data) {
                std::cerr << "ERROR: Could not load texture image file '" << charArr << "'.\n";
                width = height = 0;
            }
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    virtual color backgroundColor(const ray& r) const override {
        vec3 castedR = r.direction();

        castedR = castedR / max(max(fabs(castedR.x()), fabs(castedR.y())), fabs(castedR.z()));

        int imageReturned;
        double u,v;

        if (castedR.x() - epsilon < -1) {
            imageReturned = 0;
            u = castedR.z();
            v = castedR.y();
        } else if (castedR.x() + epsilon > 1) {
            imageReturned = 1;
            u = -castedR.z();
            v = castedR.y();;
        } else if (castedR.y() - epsilon < -1) {
            imageReturned = 2;
            u = castedR.x();
            v = castedR.z();
        } else if (castedR.y() + epsilon > 1) {
            imageReturned = 3;
            u = castedR.x();
            v = -castedR.z();
        } else if (castedR.z() - epsilon < -1) {
            imageReturned = 4;
            u = -castedR.x();
            v = castedR.y();
        } else if (castedR.z() + epsilon > 1) {
            imageReturned = 5;
            u = castedR.x();
            v = castedR.y();
        } else {
            return color(1,0,1);
        }

        if (data[imageReturned] == nullptr) {
            return color(1,0,1);
        }

        u = (u + 1.0) / 2.0;
        v = (v + 1.0) / 2.0;

        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        int i = static_cast<int>(u * width);
        int j = static_cast<int>(v * height);

        if(i >= width) {
            i = width - 1;
        }

        if(j >= height) {
            j = height - 1;
        }

        const double colorScale = 1.0/255.0;
        auto pixel = data[imageReturned] + j*bytes_per_scanline + i*bytes_per_pixel;

        return color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

    unsigned char *data[6];
    int width, height;
    int bytes_per_scanline;
};
*/

#endif //BACKGROUND_H
