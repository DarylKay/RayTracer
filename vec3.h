#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using namespace std;

//4D is used but for the sake of this, 3D is
class vec3 {
    public:
        double e[3];
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

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

        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        double length() const {
            return sqrt(lengthSquared());
        }

        double lengthSquared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            const double s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }
};

inline ostream& operator<<(ostream &os, vec3 &v) {
    return os << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unitVector(vec3 v){
    return v / v.length();
}

inline vec3 randomVec(double min, double max) {
    return vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

inline vec3 randomUnitSphere() {
    vec3 center;
    do{
        center = randomVec(-1,1);
    } while (center.length() > 1);

    return center;
}

inline vec3 lambertianScatter(vec3 normal) {
    return normal + unitVector(randomUnitSphere());
}

inline vec3 reflect(vec3 in, vec3 norm) {
    return in - 2 * dot(in, norm) * norm;
}

vec3 refract(const vec3 &in, const vec3 &n, double etaFraction) {
    vec3 inNew = unitVector(in);
    double cosTheta = dot(-inNew,n);
    cosTheta = cosTheta > 1.0 ? 1.0 : cosTheta;
    vec3 perpendicular = etaFraction * (inNew + cosTheta*n);
    vec3 parallel = -sqrt(fabs(1.0 - perpendicular.lengthSquared())) * n;
    return perpendicular + parallel;
}

inline vec3 minVals(const vec3 &a, const vec3 &b) {
    return vec3(min(a.x(),b.x()), min(a.y(),b.y()), min(a.z(),b.z()));
}

inline vec3 maxVals(const vec3 &a, const vec3 &b) {
    return vec3(max(a.x(),b.x()), max(a.y(),b.y()), max(a.z(),b.z()));
}

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(randomDouble(-1,1), randomDouble(-1,1), 0);
        if (p.lengthSquared() >= 1) continue;
        return p;
    }
}

using point3 = vec3; //3D point
using color = vec3; //RGB Color

#endif