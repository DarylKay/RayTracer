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
#include <thread>
#include "SuperSpectrum.h"
#include "RGBSpectrum.h"
#include "SampledSpectrum.h"


using namespace std;

//TO-DO

//brdf
//shadow rays
//bidirectional lighting
//mixture of sending rays from lights and sending rays from camera
//spectral imaging - light refraction and thin film spheres, chrome bubble, diamond glistening

//backup file
//kd tree
//surface area heuristic
//surface area heuristic 3 trees for if the ray is on the x plane, y plane or z plane
//point2
//cases for setting up a scene
//glossy

//spectral imaging - show the light breaking up over a cylinder

//Add a light to the scene (sun)
//fog under the dinosaur but have the fog be able to have a function of being very foggy in the middle and not foggy on the outside
//take 2 different probability functions and then depending on the x and z values, make it logarithic
//perlin noise for the fog as well, pass in a perlin noise texture

//tile texture, randomize in cos function rather than in just u and v
//Be able to randomly generate points on the fly
//BRDF - materials start with lambertian, metal and glossy, and then others

/*
Goals for the end of the semester:
-Radiance
-Spectral
-Bidirectional lighting for better lit scenes
-BRDF: fresnel, geometry, normal distribution
-BTDF
-BSDF as a mix
-Disney BSDF?
*/


time_t start;

hittable_list random_scene() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    //world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
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

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<emissive>(color(15, 15, 15));

    objects.add(make_shared<yzRect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yzRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xzRect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xzRect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xzRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xyRect>(0, 555, 0, 555, 555, white));

    return objects;
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

void calculateRow (int numRows, int j, int imageWidth, int imageHeight, vector<point3> points, camera& usingCam, hittable_list& world, background& bGround, int numSamples, int recursiveDepth, vector<color> &returnColor) {
    vector<color> retVals;

    for (int row = 0; row < numRows; ++row) {
        for (int i = 0; i < imageWidth; ++i) {
            color pixelColor(0, 0, 0);
            for (int s = 0; s < numSamples; ++s) {
                double u = (i + points[s].x()) / (imageWidth - 1);
                double v = ((j - row) + points[s].y()) / (imageHeight - 1);
                ray r = usingCam.getRay(u, v);
                pixelColor += rayColor(r, world, bGround, recursiveDepth);
            }
            retVals.emplace_back(pixelColor);
        }
    }

    returnColor = retVals;
}

int main() {
    string imageName = "CornellBoxFirst";
    int numSamples = 50;

    int processor_count = thread::hardware_concurrency();
    int threadRowSize = 10;

    bool animation = false;
    double duration = 1;
    double fps = 1;
    int frames = static_cast<int>(duration * fps);

    const int imageOption = 2; //higher = higher res

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
            imageWidth = 852;//480p
            break;
        default:
            imageWidth = 400;
            break;
    }

    const double aspectRatio = 1.0;//16.0/9.0;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    int recursiveDepth = 50;

    /*SETUP-WORLD-----------------------------------------------------------------------------------------------------*/

    auto material_light = make_shared<emissive>(color(15,15,15));
    auto material_light2 = make_shared<emissive>(color(20,20,20));


    auto white = make_shared<lambertian>(color(.73,.73,.73));


    hittable_list worldSetup = cornell_box();
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    worldSetup.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    worldSetup.add(box2);
    //hittable_list worldSetup = setupScene("assets/dinosmooth1.obj");

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


    //shared_ptr<hittable> boxFog = make_shared<box>(point3(-50,-1,-30), point3(50, 4,30), white);
    //worldSetup.add(make_shared<constant_medium>(boxFog, 0.04, color(1,1,1)));

    hittable_list world;
    world.add(make_shared<bvh_node>(worldSetup, 0, 1));

    solid_background bGround(0,0,0);
    //color background(.7,.8,1);
    //environment_map bGround("assets/Skybox-night");
    //solid_background bGround(color(.7,.8,1));

    /*CAMERA----------------------------------------------------------------------------------------------------------*/

    vec3 rotation(0,1,0);
    //point3 lookFrom(13,2,3);
    //point3 lookFrom(1.2,-.7,0);
    //point3 lookAt(0,0,0);
    point3 lookFrom = point3(278, 278, -800);
    point3 lookAt = point3(278, 278, 0);
    double focalDistance = 10.0;//(lookFrom - lookAt).length();
    double aperture = 0.0;


    camera cam(rotation, lookFrom, lookAt, aspectRatio, 40.0, focalDistance, aperture, 0,0);
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
    double estimatedTime = (difftime(endEstimate, startEstimate) * numSamples) / processor_count;

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
        for (int j = imageHeight - 1; j >= 0; j -= (4 * threadRowSize)) {
            cerr << "\rFrame: " << camNum + 1 << "/" << frames << " | Scanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j, imageHeight)
                 << ' ' << std::flush;

            vector<color> colorVector1;
            vector<color> colorVector2;
            vector<color> colorVector3;
            vector<color> colorVector4;

            thread t1 (calculateRow, threadRowSize, j - (0 * threadRowSize), imageWidth, imageHeight, ref(points), ref(usingCam), ref(world), ref(bGround), numSamples, recursiveDepth, ref(colorVector1));
            thread t2 (calculateRow, threadRowSize, j - (1 * threadRowSize), imageWidth, imageHeight, ref(points), ref(usingCam), ref(world), ref(bGround), numSamples, recursiveDepth, ref(colorVector2));
            thread t3 (calculateRow, threadRowSize, j - (2 * threadRowSize), imageWidth, imageHeight, ref(points), ref(usingCam), ref(world), ref(bGround), numSamples, recursiveDepth, ref(colorVector3));
            thread t4 (calculateRow, threadRowSize, j - (3 * threadRowSize), imageWidth, imageHeight, ref(points), ref(usingCam), ref(world), ref(bGround), numSamples, recursiveDepth, ref(colorVector4));

            t1.join();
            writeColor(image, colorVector1, numSamples);

            t2.join();
            writeColor(image, colorVector2, numSamples);

            t3.join();
            writeColor(image, colorVector3, numSamples);

            t4.join();
            writeColor(image, colorVector4, numSamples);


            /*for (int i = 0; i < imageWidth; ++i) {
                color pixelColor(0, 0, 0);
                for (int s = 0; s < numSamples / 4; ++s) {
                    double u = (i + points[s].x()) / (imageWidth - 1);
                    double v = (j + points[s].y()) / (imageHeight - 1);
                    ray r = usingCam.getRay(u, v);
                    pixelColor += rayColor(r, world, bGround, recursiveDepth);
                }
                writeColor(image, pixelColor, numSamples);
            }*/
        }
        image.close();
    }
    cout << "\nDone!" << endl;
    return 0;
}
