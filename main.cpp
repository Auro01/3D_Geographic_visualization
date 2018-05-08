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

#include "BmpLoader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shapes.hpp"
#include "mapdata.hpp"
#include "util.hpp"

unsigned int ID;
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { -5.0f, 5.0f, 19.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

glm::dmat4 projection;
glm::dmat4 camera;
glm::ivec4 view;
glm::dvec3 cameraPos;
glm::dvec2 mouseClick;

vector<Object> objects;
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

void LoadTexture(const char *filename){
    BmpLoader blLoader(filename);
    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D,ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,blLoader.iWidth,blLoader.iHeight,GL_RGB,GL_UNSIGNED_BYTE,blLoader.textureData);
}

void idle()
{
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

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
        LoadTexture("map.bmp");

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
