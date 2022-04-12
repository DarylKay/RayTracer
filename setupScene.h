#ifndef SETUPSCENE_H
#define SETUPSCENE_H

hittable_list setupScene(string file, shared_ptr<material> material_specified) {
    //read .OBJ file
    ifstream infile;
    infile.open(file);

    hittable_list scene;

    vector<point3> vertexList;
    vector<vec3> normalList;
    vector<double> u, v;

    if(infile.fail()) {
        cout << "ERROR: FILE COULD NOT BE OPENED";
        terminate();
    } else {
        string s;
        while (infile >> s) {
            if (s.compare("#") == 0) {
                getline(infile, s);
            } else if (s.compare("s") == 0) {
                getline(infile,s);
            } else if (s.compare("v") == 0) {
                double x, y, z;

                infile >> x;
                infile >> y;
                infile >> z;

                vertexList.emplace_back(x, y, z);
            } else if (s.compare("vn") == 0) {
                double x, y, z;

                infile >> x;
                infile >> y;
                infile >> z;

                normalList.emplace_back(x,y,z);
            } else if (s.compare("vt") == 0) {
                double uIn, vIn;

                infile >> uIn;
                infile >> vIn;

                u.emplace_back(uIn);
                v.emplace_back(vIn);
            } else if (s.compare("f") == 0) {
                string part1, part2, part3;
                int p1, p2, p3, n1, n2, n3, v1, v2, v3;
                vec3 normalPut[3];
                double uPut[] = {0,0,0};
                double vPut[] = {0,0,0};

                infile >> part1;
                infile >> part2;
                infile >> part3;

                p1 = stoi(part1.substr(0, part1.find('/')), nullptr, 10) - 1;
                p2 = stoi(part2.substr(0, part2.find('/')), nullptr, 10) - 1;
                p3 = stoi(part3.substr(0, part3.find('/')), nullptr, 10) - 1;

                if(!normalList.empty()) {
                    n1 = stoi(part1.substr(part1.rfind("/") + 1), nullptr, 10) - 1;
                    n2 = stoi(part2.substr(part2.rfind("/") + 1), nullptr, 10) - 1;
                    n3 = stoi(part3.substr(part3.rfind("/") + 1), nullptr, 10) - 1;

                    normalPut[0] = normalList[n1];
                    normalPut[1] = normalList[n2];
                    normalPut[2] = normalList[n3];
                }
                if(!u.empty()) {
                    v1 = stoi(part1.substr(part1.find('/') + 1, part1.rfind("/")), nullptr, 10) - 1;
                    v2 = stoi(part2.substr(part2.find('/') + 1, part2.rfind("/")), nullptr, 10) - 1;
                    v3 = stoi(part3.substr(part3.find('/') + 1, part3.rfind("/")), nullptr, 10) - 1;
                    uPut[0] = u[v1];
                    uPut[1] = u[v2];
                    uPut[2] = u[v3];

                    vPut[0] = v[v1];
                    vPut[1] = v[v2];
                    vPut[2] = v[v3];
                }

                scene.add(make_shared<triangle>(vertexList[p1],vertexList[p2],vertexList[p3], false, material_specified, uPut, vPut, true, normalPut));
            }
        }
    }

    return scene;
}


#endif
