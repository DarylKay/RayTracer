#ifndef SETUPSCENE_H
#define SETUPSCENE_H

hittable_list setupScene(string file) {
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_sphere   = make_shared<lambertian>(color(0.1,0.2,0.5));
    auto material_left  = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8,0.6,0.2), 0.0);


    //read .OBJ file
    ifstream infile;
    infile.open(file);

    hittable_list scene;

    vector<point3> vertexList;


    if(infile.fail()) {
        cout << "ERROR: FILE COULD NOT BE OPENED";
        terminate();
    } else {
        string s;
        while (infile >> s) {

            if (s.compare("v") == 0) {
                double x, y, z;

                infile >> x;
                infile >> y;
                infile >> z;

                vertexList.emplace_back(x, y, z);
            } else if (s.compare("f")) {
                string part1, part2, part3;
                int p1, p2, p3;

                infile >> part1;
                infile >> part2;
                infile >> part3;

                p1 = stoi(part1.substr(0, part1.find('/')));
                p2 = stoi(part2.substr(0, part2.find('/')));
                p3 = stoi(part3.substr(0, part3.find('/')));

                scene.add(make_shared<triangle>(vertexList[p1],vertexList[p2],vertexList[p3], false, material_sphere));
            }
        }
    }
    return scene;
}


#endif
