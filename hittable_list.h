#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>
#include <memory>

using namespace std;

class hittable_list : hittable{
public:
    hittable_list() = default;
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() {objects.clear(); }
    void add(shared_ptr<hittable> object) {objects.push_back(object); }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;


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

#endif //HITTABLE_LIST_H
