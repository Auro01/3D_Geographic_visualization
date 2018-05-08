#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <limits>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shapes.hpp"
#include "mapdata.hpp"

using namespace std;

vector<Object> objects;

glm::dmat4 projection;
glm::dmat4 camera;
glm::ivec4 view;
glm::dvec3 cameraPos;
glm::dvec2 mouseClick;

vector<MapDataPoint> data;

int subdiv = 0;
double camDist = 2.5;
double camDistStep = 0.05;

char  sNombre[100];



void saveToObj(){
    char  sAux[100];
    int len = strlen(sNombre);
    for(int i=0;i<len-4;i++){
        sAux[i]=sNombre[i];
    }
    sAux[len-4] = '.';
    sAux[len-3] = 'o';
    sAux[len-2] = 'b';
    sAux[len-1] = 'j';
    sAux[len+1] = '\0';
    cout<<"hi"<<sAux[len-4]<<"   "<<len<<endl;
    ofstream myfile(sAux);
    int iVertices = 1;
    if(myfile.is_open()){

    auto object = objects.begin();
    while(object != objects.end()) {
        for(auto & vertex : object->verteces) {
            myfile<<"v"<<" "<<vertex.x<<" "<<vertex.y<<" "<<vertex.z<<"\n";
            cout<<"v"<<" "<<vertex.x<<" "<<vertex.y<<" "<<vertex.z<<"\n";
            cout<<iVertices<<endl;
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
    else cout << "Unable to open file";
    myfile.close();

}

void mouse(int button, int state, int x, int y)
{
    switch(button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                mouseClick = glm::ivec2(x,y);
            }
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN) {
            saveToObj();
            }
            break;
        case 3: // SCROLL UP
            if(state == GLUT_DOWN) {
                camDist += camDistStep;
            }
            break;
        case 4: // SCROLL DOUW
            if(state == GLUT_DOWN) {
                camDist -= camDistStep;
                if(camDist < 0.1)
                    camDist = 0.1;
            }
            break;
    }

    cameraPos = glm::normalize(cameraPos) * camDist;
    camera = glm::lookAt(cameraPos, glm::dvec3(0,0,0), glm::dvec3(0,1,0));

    glutPostRedisplay();
}

void motion(int x, int y)
{
    auto diff = mouseClick - glm::dvec2(x, y);

    cameraPos = glm::normalize(cameraPos + glm::dvec3(diff,0.0)) * camDist;

    glutPostRedisplay();
}

void reshape(int width, int height)
{
    projection = glm::perspective(60.0, (double) width / height, 0.01, 10.0);
    view = glm::dvec4(0.0, 0.0, width, height);

    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(&projection[0][0]);

    glMatrixMode(GL_MODELVIEW);

    auto object = objects.begin();
    while(object != objects.end()) {
        glm::dmat4 modelView = camera * object->modelTrans;
        glLoadMatrixd(&modelView[0][0]);
        glBegin(GL_TRIANGLES);
            for(auto & vertex : object->verteces) {
                glVertex3d(vertex.x, vertex.y, vertex.z);
            }
        glEnd();

        ++object;
    }

    glutSwapBuffers();
}



void init() {
    double radius = 2;
    double max = -numeric_limits<double>::infinity();
    double min = numeric_limits<double>::infinity();
    double norm;

    for(auto & point : data) {
        if(point.value > max)
            max = point.value;

        if(point.value < min)
            min = point.value;
    }

    norm = 1 / (max - min);

    sphere(glm::dvec3(0,0,0), radius, subdiv, objects);

    for(auto & point : data) {
        auto theta = point.latitude * PI / 360;
        auto phi = point.longitude * PI / 360;

        auto x = radius * sin(theta) * sin(phi);
        auto y = radius * cos(theta);
        auto z = radius * sin(theta) * cos(phi);

        marker(glm::dvec3(x,y,z), point.value * norm, subdiv, objects);
    }
}

void loadFile(char * name) {
    ifstream file;
    smatch matches;
    string line;
    regex expr("(\\d+(?:\\.\\d*)?),(\\d+(?:\\.\\d*)?),(\\d+(?:\\.\\d*)?)");

    file.open(name);

    while(getline(file, line)) {
        if(regex_match(line,matches, expr)) {
            double lat = stod(string(matches[1].first, matches[1].second));
            double lng = stod(string(matches[2].first, matches[2].second));
            double val = stod(string(matches[3].first, matches[3].second));
            data.emplace_back(lat, lng, val);
        }
    }
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char **argv)
{

    cout<<"Nombre del archivo a utilizar:"<<endl;
    cin>>sNombre;
    if(sNombre != "") {
        int width = 800;
        int height = 600;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
        glutInitWindowSize(width, height);
        glutCreateWindow("GLUT");

        cameraPos = glm::dvec3(0,0,1) * camDist;
        camera = glm::lookAt(cameraPos, glm::dvec3(0,0,0), glm::dvec3(0,1,0));
        projection = glm::perspective(60.0, (double) width / height, 0.01, 10.0);
        view = glm::dvec4(0,0,width,height);

        glutMouseFunc(mouse);
        glutMotionFunc(motion);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);

        loadFile(sNombre);
        init();

        glutMainLoop();
    } else {
        cout << "No input file" << endl << endl;
        cout << "Usage: " << endl;
        cout << argv[0] << " file" << endl;
    }
    return 0;
}
