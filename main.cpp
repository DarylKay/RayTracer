#include <iostream>
#include <fstream>
using namespace std;

int main() {
    const int imageWidth = 255;
    const int imageHeight = 255;

    ofstream image;
    image.open("image.ppm", ios::out);

    image << "P3" << endl;
    image << imageWidth << ' ' << imageHeight << endl;
    image << "255" << endl;

    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            double r;
            double g;
            double b = 0.25;

            int ir = (int)(255.999 * r);
            int ig = (int)(255.999 * g);
            int ib = (int)(255.999 * b);
            image << ir << ' ' << ig << ' ' << ib << endl;
        }
    }

    image << "Hello" << endl;

    cout << "Done!" << endl;
    image.close();
    return 0;
}
