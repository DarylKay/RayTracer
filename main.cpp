#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

using namespace std;

color rayColor(const ray& r, const hittable& world) {
    hit_record rec;
    if(world.hit(r, 0, infinity, rec)){
        return 0.5*(rec.normal + color(1,1,1));
    }
    vec3 unitDirection = unitVector(r.direction());
    double t = 0.5*(unitDirection.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    camera cam;

    const double aspectRatio = 16.0/9.0;
    const int imageWidth = 400;
    const int imageHeight = (int)(imageWidth / aspectRatio);
    int numSamples = 100;

    ofstream image;
    image.open("image.ppm", ios::out);

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-1000.5,-1),1000));

    for (int j = imageHeight - 1; j >= 0; j--) {
        cerr << "\rScanlines remaining: " << j << ' ' << flush;
        for (int i = 0; i < imageWidth; i++) {
            color pixel(0,0,0);
            ray r;
            for (int k = 0; k < numSamples; k++) {
                double u = double((i + randomDouble()) / (imageWidth - 1));
                double v = double((j + randomDouble()) / (imageHeight - 1));
                r = cam.getRay(u,v);
                pixel += rayColor(r, world);
            }
            writeColor(image, pixel, numSamples);
        }
    }

    cout << "\nDone!" << endl;
    image.close();
    return 0;
}
