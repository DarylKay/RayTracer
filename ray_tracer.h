#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <memory>

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

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
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

#include "ray.h"
#include "vec3.h"

#endif //RAY_TRACER_H
