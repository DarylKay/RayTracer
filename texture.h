#ifndef TEXTURE_H
#define TEXTURE_H

#include "ray_tracer.h"
#include "ray_tracer_stb_image.h"

#include <iostream>

class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(const color& c) : colorValue(c) {}
    solid_color(double red, double green, double blue) {
        colorValue = color(red, green, blue);
    }

    virtual color value(double u, double v, const point3& p) const override{
        return colorValue;
    }

public:
    color colorValue;
};

class checker_texture : public texture {
public:
    checker_texture() {}

    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
            : even(_even), odd(_odd) {}

    checker_texture(color c1, color c2)
            : even(make_shared<solid_color>(c1)) , odd(make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3& p) const override {
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {};

    image_texture(const char *filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
                filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete data;
    }

    virtual color value(double u, double v, const point3& p) const override {
        if (data == nullptr) {
            return color(1,0,1);
        }

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
        auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

        return color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;
};

#endif //TEXTURE_H
