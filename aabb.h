#ifndef AABB_H
#define AABB_H

#include "ray_tracer.h"

class aabb {
public:
    aabb() {}
    aabb(const point3 &a, const point3 &b) {
        min = a;
        max = b;
    }

    point3 minimum() const {return min;}
    point3 maximum() const {return max;}

    point3 min;
    point3 max;

    bool hit(const ray &r, double t_min, double t_max) const;
    aabb surroundingBox(aabb one, aabb two) const;
};

inline bool aabb::hit(const ray& r, double t_min, double t_max) const {
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (minimum()[a] - r.origin()[a]) * invD;
        auto t1 = (maximum()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
            return false;
    }
    return true;
}

inline aabb aabb::surroundingBox(aabb one, aabb two) const {
    return aabb(minVals(one.minimum(),two.minimum()), maxVals(one.maximum(),two.maximum()));
}

#endif //AABB_H
