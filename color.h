#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using namespace std;

void writeColor(ostream &os, color pixelColor) {
    os << (int)(255.999 * pixelColor[0]) << ' '
       << (int)(255.999 * pixelColor[1]) << ' '
       << (int)(255.999 * pixelColor[2]) << '\n';
}

#endif //COLOR_H
