#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>
#include <cmath>

using namespace std;

void writeColor(ostream &os, SampledSpectrum pixelColor, int samples) {
    RGB rgb;
    pixelColor.ToRGB(rgb);

    //double convert = 1.0 ; (double)samples;

    double luminance = pixelColor.luminance();

    double r = rgb[0] * luminance;
    double g = rgb[1] * luminance;
    double b = rgb[2] * luminance;

    double gamma = 2.2;

    os << (int)lround(pow(clamp(r,0.0,255.0) / 255.0, gamma) * 255.0) << ' '
       << (int)lround(pow(clamp(g,0.0,255.0) / 255.0, gamma) * 255.0) << ' '
       << (int)lround(pow(clamp(b,0.0,255.0) / 255.0, gamma) * 255.0) << '\n';
}

void writeColor(ostream &os, pair<int, vector<SampledSpectrum>> pixelRow, int samples) {
        for (int i = 0; i < pixelRow.second.size(); i++) {
            writeColor(os, pixelRow.second[i], samples);
        }
        os << std::flush;
}

#endif //COLOR_H
