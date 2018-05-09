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

    strcpy(sAux, sNombre);
    strcpy(sAux + n, ".obj");

    ofstream myfile(sAux);
    int iVertices = 1;
    if(myfile.is_open()){
            cout << "Saving to file..."<<endl;
        auto object = objects.begin();
        while(object != objects.end()) {
            for(auto & vertex : object->vertices) {
                myfile<<"v"<<" "<<vertex.x<<" "<<vertex.y<<" "<<vertex.z<<"\n";
                if(iVertices%3 == 0){
                    myfile<<"\n";
                    myfile<<"f"<<" "<<iVertices-2<<" "<<iVertices-1<<" "<<iVertices<<"\n";
                    myfile<<"\n";
                }
                iVertices++;
            }
            ++object;
        }
    }
    else cout << "Unable to open file"<<endl;;
    cout << "obj exported succesfully"<<endl;
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
