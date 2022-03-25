#ifndef CAMERA_H
#define CAMERA_H

#include "ray_tracer.h"

class camera {
public:
    camera() {}

    camera(const camera &cam){
        origin = cam.origin;
        u = cam.u;
        v = cam.v;
        w = cam.w;
        horizontal = cam.horizontal;
        vertical = cam.vertical;
        origin = cam.origin;
        lowerLeftCorner = cam.lowerLeftCorner;
        lensRadius = cam.lensRadius;
        tmCamOpen = cam.tmCamOpen;
        tmCamClose = cam.tmCamClose;
    };
    camera(vec3 vup, point3 lookFrom, point3 lookAt, double aspectRatio, double vertFOVDeg, double focalDistance, double aperture, double timeOpen, double timeClose) {
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

        tmCamOpen = timeOpen;
        tmCamClose = timeClose;
    }

    ray getRay(double s, double t) const {
        vec3 inAperture = lensRadius * random_in_unit_disk();
        vec3 offset = u*inAperture.x() + v*inAperture.y();

        return ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset, randomDouble(tmCamOpen, tmCamClose));
    }

    point3 origin;
    point3 u,v,w;
    vec3 horizontal;
    vec3 vertical;
    point3 lowerLeftCorner;
    double lensRadius;
    double tmCamOpen;
    double tmCamClose;
};

#endif //CAMERA_H
