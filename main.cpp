#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <limits>
#include <cmath>
#include <cstring>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shapes.hpp"
#include "mapdata.hpp"
#include "util.hpp"

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

char sNombre[100];

void mouse(int button, int state, int x, int y)
{
    switch(button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                y = view[3] - y;
                mouseClick = planeUnproject(glm::dvec2(x, y), camera, projection, view);
            }
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN) {
                saveToObj(objects, sNombre);
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
    y = view[3] - y;
    auto diff = mouseClick - planeUnproject(glm::dvec2(x, y), camera, projection, view);
    
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

    for(auto & object : objects) {
        object.draw(camera);
    }

    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay();
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
        
        auto pos = glm::dvec3(x,y,z);
        
        marker(pos, point.value * norm, glm::normalize(pos), subdiv, objects);
    }
}

int main(int argc, char **argv)
{

    cout << "Nombre del archivo a utilizar:" << endl;
    cin >> sNombre;
    
    if(strcmp(sNombre, "") != 0) {
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

        loadFile(sNombre, data);
        init();

        glutMainLoop();
    } else {
        cout << "No input file" << endl << endl;
        cout << "Usage: " << endl;
        cout << argv[0] << " file" << endl;
    }
    return 0;
}
