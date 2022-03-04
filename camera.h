#ifndef CAMERA_H
#define CAMERA_H

#include "ray_tracer.h"

class camera {
public:
    camera(vec3 vup, point3 lookFrom, point3 lookAt, double aspectRatio, double vertFOVDeg) {
        double viewportHeight = 2.0 * tan(degToRad(vertFOVDeg) / 2.0);
        double viewportWidth = viewportHeight * aspectRatio;
        double focalLength = 1.0;

        vec3 w = unitVector(lookFrom - lookAt);
        vec3 u = unitVector(cross(vup,w));
        vec3 v = cross(w,u);

        origin = lookFrom;
        horizontal = viewportWidth * u;
        vertical = viewportHeight * v;
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - w;
    }

    ray getRay(double u, double v) {
        return ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
    }

    point3 origin;
    vec3 horizontal;
    vec3 vertical;
    point3 lowerLeftCorner;
};

#endif //CAMERA_H
