#ifndef CAMERA_H
#define CAMERA_H

#include "ray_tracer.h"

class camera {
public:
    camera() {}

    const double aspectRatio = 16.0/9.0;
    double viewportHeight = 2.0;
    double viewportWidth = viewportHeight * aspectRatio;
    double focalLength = 1.0;

    point3 origin = point3(0,0,0);
    vec3 horizontal = vec3(viewportWidth, 0, 0);
    vec3 vertical = vec3(0, viewportHeight, 0);
    point3 lowerLeftCorner = origin - horizontal/2 - vertical/2 - vec3(0,0,focalLength);

    ray getRay(double u, double v) {
        return ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
    }
};

#endif //CAMERA_H
