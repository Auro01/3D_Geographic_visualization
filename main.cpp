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
glm::dvec2 mouseClick;

vector<Object> objects;
vector<MapDataPoint> data;

int subdiv = 1;
double camDist = 3;
double camDistStep = 0.05;
double camTheta = 0;
double camPhi = 0;

char sNombre[100];

void updateCam() {
    auto x = camDist * sin(camTheta) * sin(camPhi);
    auto y = camDist * sin(camTheta) * cos(camPhi);
    auto z = camDist * cos(camTheta);

    auto pos = glm::dvec3(x,y,z);

    camera = glm::lookAt(pos, glm::dvec3(0,0,0), glm::dvec3(0,1,0));
}

void mouse(int button, int state, int x, int y)
{
    switch(button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                mouseClick = glm::dvec2(x, y);
            }
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_RIGHT_BUTTON:
            break;
        case 3: // SCROLL UP
            if(state == GLUT_DOWN) {
                camDist += camDistStep;
                updateCam();
            }
            break;
        case 4: // SCROLL DOUW
            if(state == GLUT_DOWN) {
                camDist -= camDistStep;
                if(camDist < 0.1)
                    camDist = 0.1;
                updateCam();
            }
            break;
    }


    glutPostRedisplay();
}

void motion(int x, int y)
{
    double speed = 0.1;

    auto position = glm::dvec2(x, y);

    auto diff = speed * glm::normalize(mouseClick - position);
    
    camPhi += diff.x;
    camTheta += diff.y;

    if(camTheta < - PI)
        camTheta = - PI;

    if(camTheta > PI)
        camTheta = PI;

    if(camPhi < -PI)
        camPhi = - PI;

    if(camPhi > PI)
        camPhi = PI;

    updateCam();
    
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

void loadTexture(const char *filename){
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

void createObjects(){
    objects.clear();

    double radius = 2;
    double max = -numeric_limits<double>::infinity();

    for(auto & point : data) {
        if(point.value > max)
            max = point.value;
    }

    sphere(radius, subdiv, objects);

    for(auto & point : data) {
        auto theta = point.latitude * PI / 360;
        auto phi = point.longitude * PI / 360;
        auto width = 0.2;

        auto x = radius * sin(theta) * sin(phi);
        auto y = radius * cos(theta);
        auto z = radius * sin(theta) * cos(phi);

        marker(width, (point.value / max) * 0.2, objects);

        auto & mark = objects.back();
        mark.modelTrans = glm::translate(mark.modelTrans, glm::dvec3(x, y, z));
        mark.modelTrans = glm::rotate(mark.modelTrans, point.latitude, glm::dvec3(1, 0, 0));
        mark.modelTrans = glm::rotate(mark.modelTrans, point.longitude, glm::dvec3(0, 1, 0));
    }
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

    createObjects();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :

        case 'q':
            exit(0);
            break;

        case 's':
            saveToObj(objects, sNombre);
            break;

        case GLUT_KEY_UP:
            subdiv += 1;

            if(subdiv > 10)
                subdiv = 10;

            createObjects();
            break;

        case GLUT_KEY_DOWN:
            subdiv -= 1;

            if(subdiv < 0)
                subdiv = 0;

            createObjects();
            break;
    }

    glutPostRedisplay();
}

static void specialKey(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:
            subdiv += 1;

            if(subdiv > 10)
                subdiv = 10;

            createObjects();
            break;

        case GLUT_KEY_DOWN:
            subdiv -= 1;

            if(subdiv < 0)
                subdiv = 0;

            createObjects();
            break;
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    if(argc > 1) {
        strcpy(sNombre, argv[1]);
    } else {
        cout << "Nombre del archivo a utilizar:" << endl;
        cin >> sNombre;
    }

    if(strcmp(sNombre, "") != 0) {
        int width = 800;
        int height = 600;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
        glutInitWindowSize(width, height);
        glutCreateWindow("GLUT");
        loadTexture("world.bmp");

        updateCam();
        projection = glm::perspective(60.0, (double) width / height, 0.01, 10.0);
        view = glm::dvec4(0,0,width,height);

        glutKeyboardFunc(key);
        glutSpecialFunc(specialKey);
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
