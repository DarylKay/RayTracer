#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using namespace std;

//4D is used but for the sake of this, 3D is
class vec3 {
    public:
        double e[3];
        vec3() : e{0,0,0} {
            cout << "In constructor" << endl;
        }
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {
            cout << e[0] << e[1] << e[2];
        }


        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t) {
            return *this *= 1/t;
        }


        vec3& operator+=(const vec3& v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[1];
            return *this;
        }

        double length() const {
            return sqrt(lengthSquared());
        }

        double lengthSquared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }
};

using point3 = vec3;
using color = vec3;

#endif