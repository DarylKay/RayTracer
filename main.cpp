#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "camera.h"
#include "Animator/movingCamera.h"
#include "Animator/frameMaker.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "rectangle.h"
#include <time.h>
#include <string>
#include "bvh_node.h"
#include "setupScene.h"
#include "background.h"

using namespace std;

//backup file

//kd tree
//environment mapping
//surface area heuristic
//surface area heuristic 3 trees for if the ray is on the x plane, y plane or z plane

//brdf
//point2

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

color rayColor(const ray& r, const hittable& world, const background& bGround, const int depth) {
    hit_record rec;
    if (depth <=0){
        return color(0,0,0);
    }

    if(!world.hit(r, epsilon, infinity, rec)) {
        return bGround.backgroundColor(r);
    }

    ray scattered;
    color attenuation;
    color emitted = rec.material->emitted();

    if (!rec.material->scatter(r, rec, attenuation, scattered)){
        return emitted;
    }

    return emitted + attenuation * (rayColor(scattered, world, bGround, depth - 1));
}

int main() {
    const int imageOption = 3; //higher = higher res
    int numSamples = 100;
    double duration = 4.0;
    double fps = 30;
    int frames = static_cast<int>(duration * fps);
    string imageName = "environment";

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

    auto material_light = make_shared<emissive>(color(15,15,15));
    auto material_light2 = make_shared<emissive>(color(20,20,20));


    hittable_list worldSetup;//= setupScene("dino.obj"); //= random_scene();
    //worldSetup.add(make_shared<sphere>(point3(30,80,-25), 20, material_light));
    //worldSetup.add(make_shared<sphere>(point3(-20,30,30), 8, material_light2));

    auto earth_texture = make_shared<image_texture>("assets/earth.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);

    auto ball15 = make_shared<lambertian>(make_shared<image_texture>("assets/Ball 15.jpg"));
    auto ball4 = make_shared<lambertian>(make_shared<image_texture>("assets/Ball4.jpg"));
    auto ballcue = make_shared<lambertian>(make_shared<image_texture>("assets/BallCue.jpg"));

    auto grass = make_shared<lambertian>(make_shared<image_texture>("assets/grass.jpg"));

    auto checker = make_shared<checker_texture>(color(0.1, 0.1, 0.1), color(0.9, 0.9, 0.9));
    auto material_ground = make_shared<lambertian>(color(0.042, 0.398, 0.134));
    auto material_center   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.7,0.7,0.7), 0.0);

    //worldSetup.add(make_shared<sphere>(point3( 0.0, -1000.5, -1.0), 1000, material_ground));
    //worldSetup.add(make_shared<xzRect>(-10,10,-10,10,-.5, grass));
    //worldSetup.add(make_shared<sphere>(point3( 0,    0.0, -1.0),   0.5, ball15));
    worldSetup.add(make_shared<sphere>(point3(0,    0.0, 0),   0.5, material_right));
    //worldSetup.add(make_shared<sphere>(point3( 0,    0.0, 1.0),   0.5, ballcue));

    //worldSetup.add(globe);

    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    vec3 rotation(0,1,0);
    //point3 lookFrom(5,16,18);
    point3 lookFrom(1.2,.7,0);
    point3 lookAt(0,0,0);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = 0;
    //camera cam(rotation, lookFrom, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    movingCamera movCam(rotation, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    frameMaker camLocationGenerator(lookFrom, lookAt);
    camLocationGenerator.generateYSpin(&movCam, duration, fps);

    //color background(0,0,0);
    //color background(.7,.8,1);
    environment_map bGround("assets/Skybox-day");
/*
    auto material_light = make_shared<emissive>(color(15,15,15));
    auto material_light2 = make_shared<emissive>(color(20,20,20));

    hittable_list worldSetup;// = setupScene("dino.obj"); //= random_scene();
    //worldSetup.add(make_shared<sphere>(point3(30,80,-25), 20, material_light));
    //worldSetup.add(make_shared<sphere>(point3(-20,30,30), 8, material_light2));

    auto checker = make_shared<checker_texture>(color(0.1, 0.1, 0.1), color(0.9, 0.9, 0.9));
    auto material_ground = make_shared<lambertian>(color(0.042, 0.398, 0.134));
    auto material_center   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.7,0.65,0.4), 0.0);

    auto earth_texture = make_shared<image_texture>("earth.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    //auto globe = make_shared<sphere>(point3(0, 13, 0), 5, earth_surface);

    worldSetup.add(make_shared<sphere>(point3( 0.0, -1000.5, -1.0), 1000.0, make_shared<lambertian>(checker)));
    worldSetup.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, earth_surface));
    worldSetup.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    worldSetup.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    vec3 rotation(0,1,0);
    point3 lookFrom(.1,.8,0);
    point3 lookAt(0,0,-1);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = .1;
    camera cam(rotation, lookFrom, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    //color background(0,0,0);
    color background(.7,.8,1);
*/
    time_t startEstimate = time(NULL);

    ofstream imageRough;
    stringstream imageStringEstimating;
    imageStringEstimating << "images/" << imageName << "Estimated.ppm";
    imageRough.open(imageStringEstimating.str(), ios::out);

    imageRough << "P3" << endl;
    imageRough << imageWidth << ' ' << imageHeight << endl;
    imageRough << "255" << endl;

    camera usingCam = movCam.getCamera(0);

    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rEstimating time... Scanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            double u = (i + randomDouble()) / (imageWidth-1);
            double v = (j + randomDouble()) / (imageHeight-1);
            ray r = usingCam.getRay(u, v);
            pixelColor += rayColor(r, world, bGround, maxBounce);
            writeColor(imageRough, pixelColor, 1);
        }
    }
    time_t endEstimate = time(NULL);
    double estimatedTime = difftime(endEstimate, startEstimate) * numSamples;

    start = time(NULL);

    vector<point3> points = generateSamples(2, numSamples);

    for(int camNum = 0; camNum < frames; camNum++) {
        ofstream image;
        stringstream imageString;
        imageString << "images/" << imageName << camNum << ".ppm";
        image.open(imageString.str(), ios::out); //app for append

        image << "P3" << endl;
        image << imageWidth << ' ' << imageHeight << endl;
        image << "255" << endl;

        usingCam = movCam.getCamera(camNum);

        start = time(NULL);
        for (int j = imageHeight - 1; j >= 0; --j) {
            //for (int j = 709; j >= 0; --j) {
            cerr << "\rFrame: " << camNum + 1 << "/" << frames << " | Scanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j, imageHeight)
                 << ' ' << std::flush;
            for (int i = 0; i < imageWidth; ++i) {
                color pixelColor(0, 0, 0);
                for (int s = 0; s < numSamples; ++s) {
                    double u = (i + points[s].x()) / (imageWidth - 1);
                    double v = (j + points[s].y()) / (imageHeight - 1);
                    //double u = (i + randomDouble()) / (imageWidth-1);
                    //double v = (j + randomDouble()) / (imageHeight-1);
                    ray r = usingCam.getRay(u, v);
                    pixelColor += rayColor(r, world, bGround, maxBounce);
                }
                writeColor(image, pixelColor, numSamples);
            }
        }
        image.close();
    }

    cout << "\nDone!" << endl;
    return 0;
}
