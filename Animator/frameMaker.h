#ifndef FRAMEMAKER_H
#define FRAMEMAKER_H
//calculates the camera location by a certain function or manually eventually

//spin y-lock
class frameMaker {
public:
    frameMaker(point3 _lookFrom, point3 _lookAt) {
        startLookFrom = _lookFrom;
        lookAt = _lookAt;
    }

    void generateYSpin(movingCamera *camera, double time, int fps) {
        int frames = static_cast<int>(time * fps);

        double radius = point3((startLookFrom - lookAt).x(), 0, (startLookFrom - lookAt).z()).length();

        double x,z;
        double theta;

        point3 lookFrom;

        for(int i = 0; i < frames; i++) {
            theta = (i * 2 * pi) / frames;

            x = radius * cos(theta) + lookAt.x();
            z = radius * sin(theta) + lookAt.z();

            lookFrom = point3(x, startLookFrom.y(), z);

            camera->addCamera(lookFrom);
        }
    }

    point3 startLookFrom;
    point3 lookAt;
};

#endif //FRAMEMAKER_H
