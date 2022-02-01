#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <memory>

using namespace std;

using std::shared_ptr;
using std::make_shared;

const double infinity = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double randomDouble() {
    return rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max) {
    return randomDouble() * (max - min) + min;
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#include "ray.h"
#include "vec3.h"

#endif //RAY_TRACER_H
