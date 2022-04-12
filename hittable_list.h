#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>
#include <memory>
#include "aabb.h"

using namespace std;

class hittable_list : public hittable{
public:
    hittable_list() = default;
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() {objects.clear(); }
    void add(shared_ptr<hittable> object) {objects.push_back(object); }
    void add(hittable_list list) {
        for (int i = 0; i < list.objects.size(); i++) {
            objects.push_back(list.objects[i]);
        }
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    bool boundBox(double time0, double time1, aabb& output_box) const override;

    vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    hit_record temp;
    bool anyHit = false;
    double currentClosest = t_max;

    for (auto& object : objects) {
        if (object->hit(r, t_min, currentClosest, temp)) {
            anyHit = true;
            currentClosest = temp.t;
            rec = temp;
        }
    }

    return anyHit;
}

bool hittable_list::boundBox(double time0, double time1, aabb& output_box) const {
    if (objects.empty()) {
        return false;
    }

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->boundBox(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : aabb::surroundingBox(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif //HITTABLE_LIST_H
