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
//interpolated normals barycentric coordinated (for smoothness)
//kd tree
//surface area heuristic
//texturing
//surface area heuristic 3 trees for if the ray is on the x plane, y plane or z plane

//brdf

time_t start;

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = randomDouble();
            point3 center(a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color(randomDouble(),randomDouble(),randomDouble()) * color(randomDouble(),randomDouble(),randomDouble());
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = randomVec(0.5, 1);
                    auto fuzz = randomDouble(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

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

    auto material_light = make_shared<emissive>(color(15,15,7.5));
    auto material_light2 = make_shared<emissive>(color(20,20,20));


    hittable_list worldSetup = random_scene();// = setupScene("dino.obj");
    worldSetup.add(make_shared<sphere>(point3(30,80,-25), 20, material_light));
    worldSetup.add(make_shared<sphere>(point3(-20,30,30), 8, material_light2));

/*
    auto material_ground = make_shared<lambertian>(color(0.042, 0.398, 0.134));
    auto material_center   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.7,0.65,0.4), 0.0);

    worldSetup.add(make_shared<sphere>(point3( 0.0, -1000.5, -1.0), 1000.0, material_ground));
    worldSetup.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    worldSetup.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    worldSetup.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
*/
    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    vec3 rotation(0,1,0);
    point3 lookFrom(13,2,3);
    point3 lookAt(0,0,0);
    double focalDistance = 10;
    double aperture = .2;
    camera cam(rotation, lookFrom, lookAt, aspectRatio, 25.0, focalDistance, aperture);
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
