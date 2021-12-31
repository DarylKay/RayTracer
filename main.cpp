#include <iostream>
#include <fstream>
#include "vec3.h"
#include "color.h"
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
        cerr << "\rScanlines remaining: " << j << ' ' << flush;
        for (int i = 0; i < imageWidth; i++) {
            color pixel(((double)i / (imageHeight - 1)),
                        ((double)j / (imageWidth - 1)),
                        0.25);

            writeColor(image, pixel);
        }
    }

    cout << "Done!" << endl;
    image.close();
    return 0;
}
