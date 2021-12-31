#include <iostream>
#include <fstream>
#include "vec3.h"
#include "color.h"
#include "ray.h"

using namespace std;

color rayColor(const ray &r) {
    vec3 unitDirection = unitVector(r.direction());
    double t = 0.5*(unitDirection.y() + 1.0);
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
