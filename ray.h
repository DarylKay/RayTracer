#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vec3.h"

class ray {
    public:
        point3 orig;
        vec3 dir;

        ray(){}
        ray(const point3 &origin, const vec3 &direction) : orig(origin), dir(direction) {}

        point3 origin() { return orig; }
        vec3 direction() { return dir; }

        point3 at(double t) {
            return orig + (dir * t);
        }
};

#endif //RAYTRACER_RAY_H
