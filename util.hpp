#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

#include "glm/vec3.hpp"
#include "glm/matrix.hpp"
#include "glm/gtx/transform.hpp"

#include "shapes.hpp"

using namespace std;

void saveToObj(vector<Object> & objects, char * sNombre)
{
    char sAux[100];
    auto n = strlen(sNombre) - 4;
    int vertCount = 0;

    strcpy(sAux, sNombre);
    strcpy(sAux + n, ".obj");

    ofstream myfile(sAux);
    if(myfile.is_open()){
        for(auto & object : objects) {
            for(auto & vertex : object.vertices) {
                auto p = object.modelTrans * glm::dvec4(vertex, 1.0);

                myfile << "v" << " ";
                myfile << p.x << " ";
                myfile << p.y << " ";
                myfile << p.z <<  endl;

                vertCount++;
            }
        }
        
        myfile << endl;

        for(int i = 1; i < vertCount; i += 3) {
            myfile << "f" << " ";
            myfile << i << " ";
            myfile << i + 1 << " ";
            myfile << i + 2 << endl;
        }

        cout << "Exported!" << endl;
    }
    else cout << "Unable to open file";
    myfile.close();
}

glm::dvec2 planeUnproject(const glm::dvec2 & win, glm::dmat4 & camera, glm::dmat4 & projection, glm::ivec4 & view)
{
    glm::dvec3 world1 = glm::unProject(glm::dvec3(win, 0.01), camera, projection, view);
    glm::dvec3 world2 = glm::unProject(glm::dvec3(win, 0.99), camera, projection, view);

    // u is a value such that:
    // 0 = world1.z + u * ( world2.z - world1.z )
    double u = -world1.z / ( world2.z - world1.z );
    // clamp u to reasonable values
    if( u < 0 ) u = 0;
    if( u > 1 ) u = 1;

    return glm::dvec2( world1 + u * ( world2 - world1 ) );
}

#endif