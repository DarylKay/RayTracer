#ifndef BOX_H
#define BOX_H

#include "ray_tracer.h"

#include "rectangle.h"
#include "hittable_list.h"

class box : public hittable {
public:
    box() {}
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

    virtual bool boundBox(double time0, double time1, aabb& outBox) const override {
        outBox = aabb(boxMin,boxMax);
        return true;
    }

    virtual bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const override;

    point3 boxMin;
    point3 boxMax;
    hittable_list sides;
};

box::box(const point3 &p0, const point3 &p1, shared_ptr<material> ptr) {
    boxMin = p0;
    boxMax = p1;

    sides.add(make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    sides.add(make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));

    sides.add(make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    sides.add(make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));

    sides.add(make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    sides.add(make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
}

bool box::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    return sides.hit(r,t_min,t_max,rec);
}

#endif //BOX_H
