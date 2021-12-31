#include <iostream>
#include <fstream>
#include "vec3.h"
#include "color.h"
#include "ray.h"

using namespace std;

double hitSphere(const point3 &center, double radius, const ray &r) {
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double halfB = dot(oc, r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = halfB * halfB - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        //smallest t - closest point to the camera
        return (-halfB - sqrt(discriminant)) / a;
    }
}

color rayColor(const ray &r) {
    double t = hitSphere(point3(0,0,-1), 0.50, r);
    if (t > 0.0) {
        //normal vector
        vec3 N = unitVector(r.at(t) - vec3(0,0,-1));
        return 0.5*color(N.x() + 1,N.y() + 1,N.z() + 1);
    }
    vec3 unitDirection = unitVector(r.direction());
    t = 0.5*(unitDirection.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    //image
    const double aspectRatio = 16.0/9.0;
    const int imageWidth = 400;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    ofstream image;
    image.open("image.ppm", ios::out);

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    //camera
    double viewportHeight = 2.0;
    double viewportWidth = viewportHeight * aspectRatio;
    double focalLength = 1.0;

    point3 origin = point3(0,0,0);
    vec3 horizontal = vec3(viewportWidth, 0, 0);
    vec3 vertical = vec3(0, viewportHeight, 0);
    point3 lowerLeftCorner = origin - horizontal/2 - vertical/2 - vec3(0,0,focalLength);

    for (int j = imageHeight - 1; j >= 0; j--) {
        cerr << "\rScanlines remaining: " << j << ' ' << flush;
        for (int i = 0; i < imageWidth; i++) {
            double u = double(i) / (imageWidth - 1);
            double v = double(j) / (imageHeight - 1);

            ray r(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
            color pixel = rayColor(r);

            writeColor(image, pixel);
        }
    }

    cout << "\nDone!" << endl;
    image.close();
    return 0;
}
