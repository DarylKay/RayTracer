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
#include "box.h"
#include <time.h>
#include <string>
#include "bvh_node.h"
#include "setupScene.h"
#include "background.h"
#include "constant_medium.h"

using namespace std;

//TO-DO

//backup file
//kd tree
//surface area heuristic
//surface area heuristic 3 trees for if the ray is on the x plane, y plane or z plane
//brdf
//point2
//rotate rectangles
//cases for setting up a scene

//spectral imaging - show the light breaking up over a cylinder


//fog under the dinosaur but have the fog be able to have a function of being very foggy in the middle and not foggy on the outside
//take 2 different probability functions and then depending on the x and z values, make it logarithic
//perlin noise for the fog as well, pass in a perlin noise texture

time_t start;

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
    string imageName = "dino";
    int numSamples = 20;

    bool animation = false;
    double duration = 1;
    double fps = 1;
    int frames = static_cast<int>(duration * fps);

    const int imageOption = 1; //higher = higher res

    int imageWidth;
    switch(imageOption) {
        case 4:
            imageWidth = 3840;//4k
            break;
        case 3:
            imageWidth = 1920;//1080p
            break;
        case 2:
            imageWidth = 1280;//720p
            break;
        case 1:
            imageWidth = 720;//480p
            break;
        default:
            imageWidth = 400;
            break;
    }

    const double aspectRatio = 16.0/9.0;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    int recursiveDepth = 50;

    /*SETUP-WORLD-----------------------------------------------------------------------------------------------------*/

    auto material_light = make_shared<emissive>(color(15,15,15));
    auto material_light2 = make_shared<emissive>(color(20,20,20));

    hittable_list worldSetup = setupScene("assets/dino.obj"); //= random_scene();
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
    //worldSetup.add(make_shared<sphere>(point3(0,    0.0, 0),   0.5, material_right));
    //worldSetup.add(make_shared<sphere>(point3( 0,    0.0, 1.0),   0.5, ballcue));

    //worldSetup.add(globe);

    auto white = make_shared<lambertian>(color(.73,.73,.73));

    shared_ptr<hittable> boxFog = make_shared<box>(point3(-10,-1,-5), point3(10, 2,5), white);
    worldSetup.add(make_shared<constant_medium>(boxFog, 0.01, color(1,1,1)));

    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    //color background(0,0,0);
    //color background(.7,.8,1);
    environment_map bGround("assets/Skybox-night");

    /*CAMERA----------------------------------------------------------------------------------------------------------*/

    vec3 rotation(0,1,0);
    point3 lookFrom(2,16,15);
    //point3 lookFrom(5,16,18);
    //point3 lookFrom(1.2,-.7,0);
    point3 lookAt(0,8,0);
    //point3 lookAt(0,0,0);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = 0;
    camera cam(rotation, lookFrom, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    movingCamera movCam(rotation, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    frameMaker camLocationGenerator(lookFrom, lookAt);
    camLocationGenerator.generateYSpin(&movCam, duration, fps);

    /*ESTIMATOR-------------------------------------------------------------------------------------------------------*/

    ofstream imageRough;
    stringstream imageStringEstimating;
    imageStringEstimating << "images/" << imageName << "Estimated.ppm";
    imageRough.open(imageStringEstimating.str(), ios::out);

    imageRough << "P3" << endl;
    imageRough << imageWidth << ' ' << imageHeight << endl;
    imageRough << "255" << endl;


    time_t startEstimate = time(NULL);
    camera usingCam;

    if(animation) {
        usingCam = movCam.getCamera(0);
    } else {
        usingCam = cam;
    }

    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rEstimating time... Scanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            double u = (i + randomDouble()) / (imageWidth-1);
            double v = (j + randomDouble()) / (imageHeight-1);
            ray r = usingCam.getRay(u, v);
            pixelColor += rayColor(r, world, bGround, recursiveDepth);
            writeColor(imageRough, pixelColor, 1);
        }
    }

    time_t endEstimate = time(NULL);
    double estimatedTime = difftime(endEstimate, startEstimate) * numSamples;

    /*RAYTRACER-------------------------------------------------------------------------------------------------------*/

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

        if(animation) {
            usingCam = movCam.getCamera(0);
        } else {
            usingCam = cam;
        }

        start = time(NULL);
        for (int j = imageHeight - 1; j >= 0; --j) {
            cerr << "\rFrame: " << camNum + 1 << "/" << frames << " | Scanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j, imageHeight)
                 << ' ' << std::flush;
            for (int i = 0; i < imageWidth; ++i) {
                color pixelColor(0, 0, 0);
                for (int s = 0; s < numSamples; ++s) {
                    double u = (i + points[s].x()) / (imageWidth - 1);
                    double v = (j + points[s].y()) / (imageHeight - 1);
                    ray r = usingCam.getRay(u, v);
                    pixelColor += rayColor(r, world, bGround, recursiveDepth);
                }
                writeColor(image, pixelColor, numSamples);
            }
        }
        image.close();
    }
    cout << "\nDone!" << endl;
    return 0;
}
