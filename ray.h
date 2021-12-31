#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vec3.h"

class ray {
    public:
        point3 origin;
        vec3 direction;

        ray(){}
        ray(const point3 &orig, const point) : {}
};

#endif //RAYTRACER_RAY_H
