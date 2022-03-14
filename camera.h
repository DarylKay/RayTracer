#ifndef CAMERA_H
#define CAMERA_H

#include "ray_tracer.h"

class camera {
public:
    camera(vec3 vup, point3 lookFrom, point3 lookAt, double aspectRatio, double vertFOVDeg, double focalDistance, double aperture) {
        double viewportHeight = 2.0 * tan(degToRad(vertFOVDeg) / 2.0);
        double viewportWidth = viewportHeight * aspectRatio;

        w = unitVector(lookFrom - lookAt);
        u = unitVector(cross(vup,w));
        v = cross(w,u);

        origin = lookFrom;
        horizontal = focalDistance * viewportWidth * u;
        vertical = focalDistance * viewportHeight * v;
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - focalDistance*w;

        lensRadius = aperture / 2.0;
    }

    ray getRay(double s, double t) {
        vec3 inAp = lensRadius * random_in_unit_disk();
        vec3 offset = u*inAp.x() + v*inAp.y();

        return ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
    }

    point3 origin;
    point3 u,v,w;
    vec3 horizontal;
    vec3 vertical;
    point3 lowerLeftCorner;
    double lensRadius;
};

#endif //CAMERA_H
