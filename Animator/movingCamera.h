#ifndef MOVINGCAMERA_H
#define MOVINGCAMERA_H

#include "../camera.h"
#include <vector>

//stores multiple cameraLocations and can grab them later
class movingCamera {
public:
    movingCamera(vec3 _vup, point3 _lookAt, double _aspectRatio, double _vertFOVDeg, double _focalDistance, double _aperture, double _timeOpen, double _timeClose) {
        vup = _vup;
        lookAt = _lookAt;
        aspectRatio = _aspectRatio;
        vertFOVDeg = _vertFOVDeg;
        focalDistance = _focalDistance;
        aperture = _aperture;
        timeOpen = _timeOpen;
        timeClose = _timeClose;
    }

    void addCamera(point3 lookFrom) {
        camera adding = camera(vup,lookFrom, lookAt, aspectRatio, vertFOVDeg, focalDistance, aperture, timeOpen, timeClose);
        cameraVector.push_back(adding);
    }

    camera getCamera(int camNum) {
        return cameraVector[camNum];
    }

    vector<camera> cameraVector;

    vec3 vup;
    point3 lookAt;
    double aspectRatio;
    double vertFOVDeg;
    double focalDistance;
    double aperture;
    double timeOpen;
    double timeClose;

};

#endif //MOVINGCAMERA_H
