#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"


#include "color.h"

using namespace std;

//generate samples at each pixel
//imperfect reflection
//camera
//dielectric
//emissive
//brdf

color rayColor(const ray& r, const hittable& world, const int reflectLeft) {
    hit_record rec;
    if (reflectLeft <=0){
        return color(0,0,0);
    }

    if(world.hit(r, 0.0001, infinity, rec)){
        ray scattered;
        color attenuation;

        if (rec.material->scatter(r, rec, attenuation, scattered)){
            return attenuation * (rayColor(scattered, world, reflectLeft - 1));
        }

        return color(0,0,0);
    }
    vec3 unitDirection = unitVector(r.direction());
    double t = 0.5*(unitDirection.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    camera cam;

    const int imageOption = 3; //higher = higher res
    int numSamples = 750;

    int imageWidth;
    if (imageOption == 4) {
        imageWidth = 3840;
    } else if (imageOption == 3) {
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

    shared_ptr<material> ground = make_shared<lambertian>(color(0.1,0.8,0.3));
    shared_ptr<material> reflective = make_shared<metal>(color(0.8,0.8,0.8));
    shared_ptr<material> teal = make_shared<lambertian>(color(0.01, 0.7, 0.7));
    shared_ptr<material> coral = make_shared<lambertian>(color(0.99, 0.5, 0.33));
    shared_ptr<material> maroon = make_shared<lambertian>(color(0.5, 0.01, 0.01));

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(80,40,-1001), 999, reflective));
    world.add(make_shared<sphere>(point3(0,0,1001), 999, reflective));
    world.add(make_shared<sphere>(point3(-.75,-0.34,-1.3), .6, maroon));
    world.add(make_shared<sphere>(point3(.25,-0.28,-1.3), .3, coral));
    world.add(make_shared<sphere>(point3(1.4,0.70,-1.3), .4, teal));


    //world.add(make_shared<sphere>(point3(0,-1002,-1),1000, ground));



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
