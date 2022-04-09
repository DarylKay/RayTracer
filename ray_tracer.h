#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <utility>

#include <iomanip>
#include <sstream>

using namespace std;

using std::shared_ptr;
using std::make_shared;


const double infinity = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
const double epsilon = 0.000001;

inline double degToRad(double deg) {
    return deg * (pi / 180);
}

inline double randomDouble() {
    return rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max) {
    return randomDouble() * (max - min) + min;
}

inline int randomInt(int min, int max) {
    return static_cast<int>(randomDouble(min, max+1));
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline float Lerp(float t, float v1, float v2) {
    return (1 - t) * v1 + t * v2;
}

string getTimeString(time_t start, double estimatedTime, double currentLine, double height) {
    time_t currentTime = time(NULL);

    double percentage = ((height - (currentLine + .99999))) / height;

    int diff =  (int)difftime(currentTime, start);

    int hours = diff / 3600;
    int minutes = (diff / 60) % 60;
    int seconds = diff % 60;

    int left = (int)estimatedTime;
    left = left - diff;

    percentage *= 100;

    int hoursLeft = left / 3600;
    int minutesLeft = (left / 60) % 60;
    int secondsLeft = left % 60;

    ostringstream temp;

    temp << "Percentage: " << setprecision(3) << fixed << percentage << "% | Elapsed time: ";

    if (hours < 10) {
        temp << "0";
    }
    temp << hours << ":";

    if (minutes < 10) {
        temp << "0";
    }
    temp << minutes << ":";

    if (seconds < 10) {
        temp << "0";
    }
    temp << seconds << " | Time Left: ";

    if (hoursLeft < 10) {
        temp << "0";
    }
    temp << hoursLeft << ":";

    if (minutesLeft < 10) {
        temp << "0";
    }
    temp << minutesLeft << ":";

    if (secondsLeft < 10) {
        temp << "0";
    }
    temp << secondsLeft;

    string out = temp.str();

    return out;
}

#include "vec3.h"
#include "ray.h"

vector<point3> generateSamples(int dimension, int samples) {
    vector<point3> genSamples;

    int prime[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};

    double p[samples];
    double h[samples * dimension];

    double lognsamples = log(samples + 1);

    for (int i = 0; i < dimension; i++) {
        int b = prime[i];
        int n = static_cast<int>(ceil(lognsamples / log(b)));
        for (int t = 0; t < n; t++) {
            p[t] = pow(b, -(t + 1));
        }

        for (int j = 0; j < samples; j++) {
            int d = j + 1;
            double sum = fmod(d, b) * p[0];
            for (int t = 1; t < n; t++) {
                d = floor(d / b);
                sum += fmod(d, b) * p[t];
            }

            h[j * dimension + i] = sum;
        }
    }

    //works for 2D
    for (int i = 0; i < samples * dimension; i += dimension) {
        genSamples.push_back(point3(h[i], h[i + 1], 0));
    }

    return genSamples;
}

#endif //RAY_TRACER_H
