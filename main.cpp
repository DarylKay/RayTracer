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

#include "setupScene.h"


using namespace std;

//backup file
//camera special effects
//finish dielectric
//emissive
//brdf

time_t start;

color rayColor(const ray& r, const hittable& world, const int reflectLeft) {
    hit_record rec;
    if (reflectLeft <=0){
        return color(0,0,0);
    }

    if(world.hit(r, epsilon, infinity, rec)){
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
    const int imageOption = 3; //higher = higher res
    int numSamples = 500;

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

    ofstream imageRough;
    imageRough.open("imageRough.ppm", ios::out);

    imageRough << "P3" << endl;
    imageRough << imageWidth << ' ' << imageHeight << endl;
    imageRough << "255" << endl;

    hittable_list world = setupScene("sphere.obj");

    /* dielectric test
    auto material_ground = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<lambertian>(color(0.8, 0.6, 0.2));
    auto material_triangle  = make_shared<metal>(color(0.8,0.8,0.8), 0.0);
    auto material_center  = make_shared<dielectric>(1.5);


    world.add(make_shared<triangle>(point3(10,-0.5,10),point3(0,-0.5,-10), point3(-10,-0.5, 10), false, material_triangle));
    //world.add(make_shared<triangle>(point3(-100,-0.5, 100),point3(0,-0.5,-100),point3(100,-0.5,100), true, material_left));
    //world.add(make_shared<triangle>(point3(-1,-0.5, -3),point3(0,1,-3),point3(1,-0.5,-3), true, material_right));
    world.add(make_shared<sphere>(point3( 0.0,    0, -1.0),   0.5, material_ground));
    //world.add(make_shared<sphere>(point3(-1.1,    0.0, -1.0),   0.5, material_left));
    //world.add(make_shared<sphere>(point3( 1.1,    0.0, -1.0),   0.5, material_right));
*/

    /*
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8,0.6,0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
*/

    vec3 rotation(0,1,0);
    point3 lookAt(0,0,-1);
    point3 lookFrom(0,0,3);
    camera cam(rotation, lookFrom, lookAt, aspectRatio, 90.0);

   time_t startEstimate = time(NULL);
    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rEstimating time... Scanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            double u = (i + randomDouble()) / (imageWidth-1);
            double v = (j + randomDouble()) / (imageHeight-1);
            ray r = cam.getRay(u, v);
            pixelColor += rayColor(r, world, maxBounce);
            writeColor(imageRough, pixelColor, 1);
        }
    }
    time_t endEstimate = time(NULL);
    double estimatedTime = difftime(endEstimate, startEstimate) * numSamples;

    start = time(NULL);

    for (int j = imageHeight-1; j >= 0; --j) {
        cerr <<"\rScanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j,imageHeight) << ' ' <<  std::flush;
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
