#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using namespace std;

void writeColor(ostream &os, SampledSpectrum pixelColor, int samples) {
    RGB rgb;
    pixelColor.ToRGB(rgb);

    //float convert = 1.0 ; (float)samples;

    float luminance = pixelColor.luminance();

    float r = static_cast<float>(rgb[0]);
    float g = static_cast<float>(rgb[1]);
    float b = static_cast<float>(rgb[2]);

    //color raised to 1/gamma, gamma=2.0
    r = sqrt(r);
    g = sqrt(g);
    b = sqrt(b);


    os << (int)(255.999 * clamp(r,0.0,1.0)) << ' '
       << (int)(255.999 * clamp(g,0.0,1.0)) << ' '
       << (int)(255.999 * clamp(b,0.0,1.0)) << '\n';
}

void writeColor(ostream &os, vector<SampledSpectrum> pixelRow, int samples) {
        for (int i = 0; i < pixelRow.size(); i++) {
            writeColor(os, pixelRow[i], samples);
        }
        os << std::flush;
}

#endif //COLOR_H
