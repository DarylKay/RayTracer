#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "time.h"
#include <string>
#include "bvh_node.h"
#include "setupScene.h"

using namespace std;

//backup file

//camera special effects
//finish dielectric
//emissive
//interpolated normals barycentric coordinated (for smoothness)
//kd tree
//surface area heuristic
//texturing

//brdf

time_t start;

color rayColor(const ray& r, const hittable& world, const color& background, const int depth) {
    hit_record rec;
    if (depth <=0){
        return color(0,0,0);
    }

    if(!world.hit(r, epsilon, infinity, rec)) {
        return background;
    }

    ray scattered;
    color attenuation;
    color emitted = rec.material->emitted();

    if (!rec.material->scatter(r, rec, attenuation, scattered)){
        return emitted;
    }

    return emitted + attenuation * (rayColor(scattered, world, background, depth - 1));
}

int main() {
    const int imageOption = 3; //higher = higher res
    int numSamples = 1000;

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
    image.open("image.ppm", ios::out); //app for append

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    ofstream imageRough;
    imageRough.open("imageRough.ppm", ios::out);

    imageRough << "P3" << endl;
    imageRough << imageWidth << ' ' << imageHeight << endl;
    imageRough << "255" << endl;

    auto material_light = make_shared<emissive>(color(10,10,10));

    hittable_list worldSetup;// = setupScene("dino.obj");
    worldSetup.add(make_shared<sphere>(point3(20,80,-18), 20, material_light));

    auto material_ground = make_shared<lambertian>(color(0.042, 0.398, 0.134));
    auto material_center   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.7,0.65,0.4), 0.0);

    worldSetup.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    worldSetup.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    worldSetup.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    worldSetup.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    vec3 rotation(0,1,0);
    point3 lookAt(0,0,-1);
    point3 lookFrom(0,.75,0);
    camera cam(rotation, lookFrom, lookAt, aspectRatio, 90.0);
    color background(0,0,0);
    //color background(.7,.8,1);

    time_t startEstimate = time(NULL);
    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rEstimating time... Scanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            double u = (i + randomDouble()) / (imageWidth-1);
            double v = (j + randomDouble()) / (imageHeight-1);
            ray r = cam.getRay(u, v);
            pixelColor += rayColor(r, world, background, maxBounce);
            writeColor(imageRough, pixelColor, 1);
        }
    }
    time_t endEstimate = time(NULL);
    double estimatedTime = difftime(endEstimate, startEstimate) * numSamples;

    start = time(NULL);

    for (int j = imageHeight-1; j >= 0; --j) {
    //for (int j = 709; j >= 0; --j) {
        cerr <<"\rScanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j,imageHeight) << ' ' <<  std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            for (int s = 0; s < numSamples; ++s) {
                double u = (i + randomDouble()) / (imageWidth-1);
                double v = (j + randomDouble()) / (imageHeight-1);
                ray r = cam.getRay(u, v);
                pixelColor += rayColor(r, world, background, maxBounce);
            }
            writeColor(image, pixelColor, numSamples);
        }
    }

    cout << "\nDone!" << endl;
    image.close();
    return 0;
}
