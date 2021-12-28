#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using namespace std;

//4D is used but for the sake of this, 3D is
class vec3 {
    public:
        double e[3];
        vec3() : e{0,0,0} {
            cout << "In constructor" << endl;
        }
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {
            cout << e[0] << e[1] << e[2];
        }

        //indexing
        //address
        //xyz
        //negative
        // divide
        //multiply operator
        //add operator (two vectors)
        //length function
        //length squared function

};

using point3 = vec3;
using color = vec3;

#endif