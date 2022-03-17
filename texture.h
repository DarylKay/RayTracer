#ifndef TEXTURE_H
#define TEXTURE_H

#include "ray_tracer.h"

class texture {
    virtual color value(double u, double v, point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(color& c) : colorValue(c) {}
    solid_color(double red, double green, double blue) {
        colorValue = color(red, green, blue);
    }

    virtual color value(double u, double v, point3& p) const override{
        return colorValue;
    }

    color colorValue;
};

#endif //TEXTURE_H
