#include <iostream>
#include <fstream>
#include "vec3.cpp";
using namespace std;

int main() {
    const int imageWidth = 256;
    const int imageHeight = 256;

    ofstream image;
    image.open("image.ppm", ios::out);

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    for (int j = imageHeight - 1; j >= 0; j--) {
        cout << "\rScanlines remaining: " << j<< ' ' << flush;
        for (int i = 0; i < imageWidth; i++) {
            double r = ((double)i / (imageHeight - 1));
            double g = ((double)j / (imageWidth - 1));
            double b = 0.25;

            int ir = (int)(255.999 * r);
            int ig = (int)(255.999 * g);
            int ib = (int)(255.999 * b);
            image << ir << ' ' << ig << ' ' << ib << endl;
        }
    }

    const point3 point1 = point3();
    cout << "Point 1 done" << endl;
    point3 point2 = point3(1,2,3);
    cout << "Point 2 done" << endl;

    cout << "Done!" << endl;
    image.close();
    return 0;
}
