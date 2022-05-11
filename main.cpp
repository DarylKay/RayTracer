#include <iostream>
#include <fstream>

#include "ray_tracer.h"

#include "SuperSpectrum.h"
#include "SampledSpectrum.h"

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
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <algorithm>


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

/*
 * RGB color to color spectrum, sample over the red blue and green wave length at every single pixel.
 * Those colors and added values are added to a color spectrum. That spectrum is then converted to rgb later
 * Wavelength are refracted based on the wavelength as well
 *
 */

time_t start;

hittable_list random_scene() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(RGB(0.5, 0.5, 0.5));
    //world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = randomDouble();
            point3 center(a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = RGB(randomDouble(),randomDouble(),randomDouble()) * RGB(randomDouble(),randomDouble(),randomDouble());
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
                    sphere_material = make_shared<dielectric>(1.5, 0);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    auto material1 = make_shared<dielectric>(1.5, 0);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    auto material2 = make_shared<lambertian>(RGB(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
    auto material3 = make_shared<metal>(RGB(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    return world;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(RGB(.65, .05, .05));
    auto white = make_shared<lambertian>(RGB(.73, .73, .73));
    auto green = make_shared<lambertian>(RGB(.12, .45, .15));
    auto light = make_shared<emissive>(RGB(15, 15, 15));

    objects.add(make_shared<yzRect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yzRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xzRect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xzRect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xzRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xyRect>(0, 555, 0, 555, 555, white));

    return objects;
}

double rayColor(const ray& r, const hittable& world, const background& bGround, const int depth, const double lambda) {
    hit_record rec;
    if (depth <=0){
        return 0;
    }

    if(!world.hit(r, epsilon, infinity, rec)) {
        return bGround.backgroundColor(r, lambda);
    }

    ray scattered;
    double attenuation;
    double emitted = rec.material->emitted(lambda);

    if (!rec.material->scatter(r, rec, attenuation, scattered, lambda)){
        return emitted;
    }

    return emitted + attenuation * (rayColor(scattered, world, bGround, depth - 1, lambda));
}

void calculateRow (int numRows, int j, int imageWidth, int imageHeight, vector<point3> points, camera& usingCam, hittable_list& world, background& bGround, int numSamples, int recursiveDepth, vector<pair<int, vector<SampledSpectrum>>> &returnBlocks, mutex &mutex, condition_variable &results, atomic<int> &threadsCompleted) {
    vector<SampledSpectrum> retVals;

    for (int row = 0; row < numRows; ++row) {
        for (int i = 0; i < imageWidth; ++i) {
            vector<pair<double,double>> samplingSpectrum;
            for (int s = 0; s < numSamples; ++s) {
                double u = (i + points[s].x()) / (imageWidth - 1);
                double v = ((j - row) + points[s].y()) / (imageHeight - 1);

                ray r = usingCam.getRay(u, v);

                double R = Lerp(points[s].z(), sampledLambdaStart, sampledLambaEnd);
                samplingSpectrum.push_back(make_pair(R,rayColor(r, world, bGround, recursiveDepth, R)));
            }

            SampledSpectrum pixelSpectrum = SampledSpectrum::FromSampled(samplingSpectrum);
            retVals.emplace_back(pixelSpectrum);

        }
        cerr << "\rDone with row:" << j - row<< ' ' << endl <<std::flush;
    }

    lock_guard<std::mutex> lock(mutex);
    threadsCompleted++;
    results.notify_one();
    returnBlocks.push_back(make_pair(j, retVals));
}

int main() {
    SampledSpectrum::Init();
/*
    string imageName = "GlassPrism";
    int numSamples = 100;

    int processor_count = thread::hardware_concurrency();
    int threadRowSize = 10;

    bool animation = false;
    double duration = 1;
    double fps = 1;
    int frames = static_cast<int>(duration * fps);

    const int imageOption = 3; //higher = higher res

    int imageWidth;
    switch(imageOption) {
        case 5:
            imageWidth = 7680;//8k
            break;
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
            imageWidth = 480;
            break;
    }

    const double aspectRatio = 16.0/9.0;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    int recursiveDepth = 50;
*/

    string imageName = "Diamond4k10000Samples";
    int numSamples = 10000;

    mutex mutex;
    condition_variable results;
    vector<pair<int, vector<SampledSpectrum>>> pixelBlocks;
    atomic<int> threadsCompleted = {0};
    vector<thread> threads;

    int processor_count = thread::hardware_concurrency();
    int threadRowSize = 10;

    bool animation = false;
    double duration = 1;
    double fps = 1;
    int frames = static_cast<int>(duration * fps);

    const int imageOption = 4; //higher = higher res

    int imageWidth;
    switch(imageOption) {
        case 5:
            imageWidth = 7680;//8k
            break;
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
            imageWidth = 854;//480p
            break;
        default:
            imageWidth = 480;
            break;
    }


    const double aspectRatio = 16.0/9.0;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    int recursiveDepth = 50;

    /*SETUP-WORLD-----------------------------------------------------------------------------------------------------*/
/*    auto material_light = make_shared<emissive>(RGB(10000,10000,10000));

    auto white = make_shared<lambertian>(RGB(.73,.73,.73));

    hittable_list worldSetup;

    auto flintGlass = make_shared<dielectric>(1.520, 0.01342);
    hittable_list diamondList = setupScene("assets/triprism.obj", flintGlass);
    auto whiteLamb = make_shared<lambertian>(RGB(.99,.99,.99));
    //hittable_list backdropList = setupScene("assets/backdrop.obj", whiteLamb);

    worldSetup.add(diamondList);
    worldSetup.add(make_shared<xzRect>(-50, 50, -50,50,0, whiteLamb));
    //worldSetup.add(backdropList);

    worldSetup.add(make_shared<sphere>(point3(-6,3,0), .08, material_light));
    //worldSetup.add(make_shared<sphere>(point3(0,5,0), .5, material_light));
*/

    auto material_light = make_shared<emissive>(RGB(200,200,200));

    //auto material_light = make_shared<emissive>(RGB(350,350,350));

    auto white = make_shared<glossy>(RGB(.73,.73,.73), .5);

    hittable_list worldSetup;

    auto diamond = make_shared<dielectric>(2.4218, 0.0121);
    hittable_list diamondList = setupScene("assets/dimondUp.obj", diamond);
    auto whiteLamb = make_shared<lambertian>(RGB(.99,.99,.99));
    //hittable_list backdropList = setupScene("assets/backdrop.obj", whiteLamb);

    worldSetup.add(diamondList);
    worldSetup.add(make_shared<xzRect>(-40, 40, -40,40,0, whiteLamb));
    //worldSetup.add(backdropList);


    worldSetup.add(make_shared<sphere>(point3(0,3.5,5.25), .8, material_light));

    //worldSetup.add(make_shared<sphere>(point3(-1.125,4.5,5.25), .9, material_light));
    //worldSetup.add(make_shared<sphere>(point3(0,5,0), .5, material_light));

/*
    auto earth_texture = make_shared<image_texture>("assets/earth.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);

    auto ball15 = make_shared<lambertian>(make_shared<image_texture>("assets/Ball 15.jpg"));
    auto ball4 = make_shared<lambertian>(make_shared<image_texture>("assets/Ball4.jpg"));
    auto ballcue = make_shared<lambertian>(make_shared<image_texture>("assets/BallCue.jpg"));

    auto grass = make_shared<lambertian>(make_shared<image_texture>("assets/grass.jpg"));
*/
    //auto checker = make_shared<checker_texture>(color(0.1, 0.1, 0.1), color(0.9, 0.9, 0.9));
    auto material_ground = make_shared<lambertian>(RGB(0.042, 0.398, 0.134));
    auto material_center   = make_shared<lambertian>(RGB(0.9,0.5,0.5));
    auto material_left  = make_shared<dielectric>(1.522, 0.04059);
    auto material_right  = make_shared<metal>(RGB(0.7,0.7,0.7), 0.0);

    //worldSetup.add(make_shared<sphere>(point3( 0.0, -1000.5, -1.0), 1000, material_ground));
    /*
    worldSetup.add(make_shared<xzRect>(-10,10,-10,10,-.5, material_ground));
    worldSetup.add(make_shared<sphere>(point3( -1,    0.0, 1),   0.5, material_left));
    worldSetup.add(make_shared<sphere>(point3(0,    0.0, 1),   0.5, material_center));
    worldSetup.add(make_shared<sphere>(point3( 1,    0.0, 1),   0.5, material_right));
*/
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
    point3 lookFrom(0,2.1783,3.475);
    point3 lookAt(0,0,0);
    //point3 lookFrom = point3(278, 278, -800);
    //point3 lookAt = point3(278, 278, 0);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = 0.01;


    camera cam(rotation, lookFrom, lookAt, aspectRatio, 50.0, focalDistance, aperture, 0,0);
    movingCamera movCam(rotation, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    frameMaker camLocationGenerator(lookFrom, lookAt);
    camLocationGenerator.generateYSpin(&movCam, duration, fps);

    /*
    vec3 rotation(0,1,0);
    //point3 lookFrom(13,2,3);
    point3 lookFrom(.5,2.7783,3.475);
    point3 lookAt(0,0,0);
    //point3 lookFrom = point3(278, 278, -800);
    //point3 lookAt = point3(278, 278, 0);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = 0.01;


    camera cam(rotation, lookFrom, lookAt, aspectRatio, 60.0, focalDistance, aperture, 0,0);
    movingCamera movCam(rotation, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    frameMaker camLocationGenerator(lookFrom, lookAt);
    camLocationGenerator.generateYSpin(&movCam, duration, fps);
     */
/*

    vec3 rotation(0,1,0);
    //point3 lookFrom(13,2,3);
    point3 lookFrom(1.35,4,7.5);
    point3 lookAt(1,0,1);
    //point3 lookFrom = point3(278, 278, -800);
    //point3 lookAt = point3(278, 278, 0);
    double focalDistance = (lookFrom - lookAt).length();
    double aperture = 0.01;


    camera cam(rotation, lookFrom, lookAt, aspectRatio, 45, focalDistance, aperture, 0,0);
    movingCamera movCam(rotation, lookAt, aspectRatio, 90.0, focalDistance, aperture, 0,0);
    frameMaker camLocationGenerator(lookFrom, lookAt);
    camLocationGenerator.generateYSpin(&movCam, duration, fps);
*/
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
/*
    for (int j = imageHeight-1; j >= 0; --j) {
        cerr << "\rEstimating time... Scanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            vector<pair<double,double>> samplingSpectrum;
            double u = (i + randomDouble()) / (imageWidth-1);
            double v = (j + randomDouble()) / (imageHeight-1);

            ray r = usingCam.getRay(u, v);
            double R = randomDouble(sampledLambdaStart, sampledLambaEnd);
            samplingSpectrum.emplace_back(R,rayColor(r, world, bGround, recursiveDepth, R));
            double G = randomDouble(sampledLambdaStart, sampledLambaEnd);
            samplingSpectrum.emplace_back(G,rayColor(r, world, bGround, recursiveDepth, G));
            double B = randomDouble(sampledLambdaStart, sampledLambaEnd);
            samplingSpectrum.emplace_back(B,rayColor(r, world, bGround, recursiveDepth, B));

            SampledSpectrum pixelSpectrum = SampledSpectrum::FromSampled(samplingSpectrum);

            writeColor(imageRough, pixelSpectrum, 1);
        }
    }
*/
    time_t endEstimate = time(NULL);
    double estimatedTime = (difftime(endEstimate, startEstimate) * numSamples) / processor_count;

    /*RAYTRACER-------------------------------------------------------------------------------------------------------*/

    start = time(NULL);
    vector<point3> points = generateSamples(3, numSamples);

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
        for (int j = imageHeight - 1; j >= 0; j -= 1/*(4 * threadRowSize)*/) {
            cerr << "\rFrame: " << camNum + 1 << "/" << frames << " | Scanlines remaining: " << j << " | " << getTimeString(start, estimatedTime, j, imageHeight)
                 << ' ' << std::flush;
/*
            threadsCompleted = 0;
            threads.clear();
            pixelBlocks.clear();

            for (int i = 0; i < processor_count; ++i) {
                vector<pair<int, SampledSpectrum>> block;
                int start = j - (i * threadRowSize);
                thread t([threadRowSize, start, imageWidth, imageHeight, &points, &usingCam, &world, &bGround, numSamples, recursiveDepth, &pixelBlocks, &mutex, &results, &threadsCompleted]() {
                    calculateRow(threadRowSize, start, imageWidth, imageHeight, points, usingCam, world, bGround, numSamples, recursiveDepth, pixelBlocks, mutex, results, threadsCompleted);
                });
                threads.push_back(move(t));
            }

            unique_lock<std::mutex> lock(mutex);
            results.wait(lock, [&threadsCompleted, &processor_count] {
                return threadsCompleted == processor_count;
            });

            for (std::thread& t : threads)
            {
                t.join();
            }

            sort(pixelBlocks.begin(), pixelBlocks.end(), sort_);

            reverse(pixelBlocks.begin()->first, pixelBlocks.end()->first);

            for (int i = 0; i < processor_count; i++) {
                writeColor(image, pixelBlocks[i], numSamples);
            }
            */


            for (int i = 0; i < imageWidth; ++i) {
                vector<pair<double,double>> samplingSpectrum;
                for (int s = 0; s < numSamples; ++s) {
                    double u = (i + points[s].x()) / (imageWidth - 1);
                    double v = (j + points[s].y()) / (imageHeight - 1);
                    ray r = usingCam.getRay(u, v);

                    double R = Lerp( points[s].z(), sampledLambdaStart, sampledLambaEnd);
                    samplingSpectrum.push_back(make_pair(R,rayColor(r, world, bGround, recursiveDepth, R)));
                }
                SampledSpectrum pixelSpectrum = SampledSpectrum::FromSampled(samplingSpectrum);

                writeColor(image, pixelSpectrum, numSamples);
            }

        }

        image.close();
    }
    cout << "\nDone!" << endl;
    return 0;
}
