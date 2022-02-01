#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using namespace std;

void writeColor(ostream &os, color pixelColor, int samples) {
    double r = pixelColor[0];
    double g = pixelColor[1];
    double b = pixelColor[2];

    double convert = 1.0 / (double)samples;

    //color raised to 1/gamma, gamma=2.0
    r = sqrt(r * convert);
    g = sqrt(g * convert);
    b = sqrt(b * convert);

    os << (int)(255.999 * clamp(r,0.0,1.0)) << ' '
       << (int)(255.999 * clamp(g,0.0,1.0)) << ' '
       << (int)(255.999 * clamp(b,0.0,1.0)) << '\n';
}

#endif //COLOR_H
