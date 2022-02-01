#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

using namespace std;

color rayColor(const ray& r, const hittable& world, const int reflectLeft) {
    hit_record rec;
    if (reflectLeft <=0){
        return color(0,0,0);
    }

    if(world.hit(r, 0.000001, infinity, rec)){
        vec3 bounce = rec.p + rec.normal + randomUnitSphere();
        return 0.5 * (rayColor(ray(rec.p, bounce - rec.p), world, reflectLeft - 1));
    }
    vec3 unitDirection = unitVector(r.direction());
    double t = 0.5*(unitDirection.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    camera cam;

    const int imageOption = 2; //higher = higher res
    int numSamples = 1000;

    int imageWidth;
    if (imageOption == 3) {
        imageWidth = 1920;
    } else if (imageOption == 2) {
        imageWidth = 1280;
    } else if (imageOption == 1) {
        imageWidth = 720;
    } else {
        imageWidth = 400;
    }

    const double aspectRatio = 16.0/9.0;
    const int imageHeight = (int)(imageWidth / aspectRatio);
    int maxBounce = 50;

    ofstream image;
    image.open("image.ppm", ios::out);

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-1000.5,-1),1000));
    world.add(make_shared<sphere>(point3(-0.45,0,-1),0.25));
    world.add(make_shared<sphere>(point3(0.45,0,-1),0.25));
    world.add(make_shared<sphere>(point3(-0.15,0.05,-0.5),0.1));
    world.add(make_shared<sphere>(point3(0.15,0.05,-0.5),0.1));
    world.add(make_shared<sphere>(point3(-2.25,0,-2),0.5));
    world.add(make_shared<sphere>(point3(2.25,0,-2),0.5));



    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            for (int s = 0; s < numSamples; ++s) {
                double u = (i + randomDouble()) / (imageWidth-1);
                double v = (j + randomDouble()) / (imageHeight-1);
                ray r = cam.getRay(u, v);
                pixelColor += rayColor(r, world, maxBounce);
            }
            writeColor(image, pixelColor, numSamples);
        }
    }

    cout << "\nDone!" << endl;
    image.close();
    return 0;
}
