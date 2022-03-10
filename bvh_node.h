#ifndef BVH_H
#define BVH_H

#include "ray_tracer.h"
#include "algorithm"

#include "hittable.h"
#include "hittable_list.h"

class bvh_node : public hittable {
public:
    bvh_node(){};
    bvh_node(hittable_list& list, double t0, double t1) : bvh_node(list.objects, 0, list.objects.size(), t0, t1){};
    bvh_node(vector<shared_ptr<hittable>> objects, size_t start, size_t end, double time0, double time1){
        auto copyObjects = objects;

        auto span = end - start;

        int random = randomInt(0,2);
        auto comparator = (random == 0) ? box_x_compare : (random == 1) ? box_y_compare : box_z_compare;

        if (random == 0) {
            comparator = box_x_compare;
        } else if (random == 1) {
            comparator = box_y_compare;
        } else {
            comparator = box_z_compare;
        }

        if(span == 1) {
            left = copyObjects[start];
            right = copyObjects[start];
        } else if (span == 2) {
            if (comparator(copyObjects[start], copyObjects[start + 1])){
                left = copyObjects[start];
                right = copyObjects[start + 1];
            } else {
                left = copyObjects[start + 1];
                right = copyObjects[start];
            }
        } else {
            std::sort(copyObjects.begin() + start, copyObjects.begin() + end, comparator);

            auto mid = start + span/2;
            left = make_shared<bvh_node>(objects, start, mid, time0, time1);
            right = make_shared<bvh_node>(objects, mid, end, time0, time1);
        }

        aabb boxLeft, boxRight;

        if (!left->boundBox (time0, time1, boxLeft) || !right->boundBox(time0, time1, boxRight)){
            std::cerr << "No bounding box in bvh_node constructor.\n";
            terminate();
        }

        boundingBox = aabb::surroundingBox(boxLeft, boxRight);
    }

    static inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
        aabb box_a;
        aabb box_b;

        if (!a->boundBox(0,0, box_a) || !b->boundBox(0,0, box_b)) {
            std::cerr << "No bounding box in bvh_node constructor.\n";
            terminate();
        }

        return box_a.minimum().e[axis] < box_b.minimum().e[axis];
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool boundBox(double t_min, double t_max, aabb& box) const override;

    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb boundingBox;
};

bool bvh_node::boundBox(double t_min, double t_max, aabb& box) const {
    box = boundingBox;
    return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if(!boundingBox.hit(r, t_min, t_max)) {
        return false;
    }

    bool hitLeft = left->hit(r, t_min, t_max, rec);
    bool hitRight;
    if (hitLeft) {
        hitRight = right->hit(r, t_min, rec.t, rec);
    } else {
        hitRight = right->hit(r, t_min, t_max, rec);
    }

    return hitLeft || hitRight;
}




#endif //BVH_H
