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

    if(world.hit(r, 0, infinity, rec)){
        vec3 bounce = rec.p + rec.normal + randomUnitSphere();
        return 0.5 * (rayColor(ray(rec.p, bounce - rec.p), world, reflectLeft - 1));
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
    int numSamples = 5;
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

    static color imageDS[imageWidth][imageHeight];

    for(int samp = 0; samp < numSamples; ++samp) {
        cerr << "\rSamples done: " << samp << "/" << numSamples << flush;
        for (int j = imageHeight - 1; j >= 0; j--) {
            for (int i = 0; i < imageWidth; i++) {
                double u = (i + randomDouble()) / (imageWidth - 1.0);
                double v = (j + randomDouble()) / (imageHeight - 1.0);
                ray r = cam.getRay(u, v);
                imageDS[i][j] = imageDS[i][j] + rayColor(r, world, maxBounce);
            }
        }
    }

    cout << "\nDone!" << endl;
    image.close();
    return 0;
}
