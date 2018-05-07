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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shapes.hpp"
#include "mapdata.hpp"

using namespace std;

vector<Object> objects;

glm::dmat4 projection;
glm::ivec4 view;
glm::dmat4 camera;

glm::dvec3 cameraPos;
glm::dvec2 mousePos;

vector<MapDataPoint> data;

// unprojects the given window point
// and finds the ray intersection with the Z=0 plane
glm::dvec2 PlaneUnproject( const glm::dvec2& win )
{
    glm::dvec3 world1 = glm::unProject(glm::dvec3(win, 0.01), camera, projection, view);
    glm::dvec3 world2 = glm::unProject(glm::dvec3(win, 0.99), camera, projection, view);

    // u is a value such that:
    // 0 = world1.z + u * ( world2.z - world1.z )
    double u = -world1.z / ( world2.z - world1.z );
    
    // clamp u to reasonable values
    if(u < 0)
        u = 0;
    else if(u > 1)
        u = 1;

    return glm::dvec2( world1 + u * ( world2 - world1 ) );
}

void applyCamera()
{
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixd(&projection[0][0]);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd(&camera[0][0]);
}

void mouse( int button, int state, int x, int y )
{
    glutPostRedisplay();
}

void motion( int x, int y )
{
    glutPostRedisplay();
}

void passiveMotion( int x, int y )
{
    glutPostRedisplay();
}

void wheel( int wheel, int direction, int x, int y )
{
    glutPostRedisplay();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    applyCamera();

    auto object = objects.begin();
    while(object != objects.end()) {
        glPushMatrix();
        glLoadMatrixd(&object->modelTrans[0][0]);
        glBegin(GL_TRIANGLES);
            for(auto vertex : object->verteces) {
                glVertex3d(vertex.x, vertex.y, vertex.z);
            }
        glEnd();
        glPopMatrix();

        ++object;
    }

    glutSwapBuffers();
}

void loadFile(char * name) {
    ifstream file;
    file.open(name);

    string line;

    while(getline(file, line)) {
        auto lineStream = stringstream(line);
        string latStr;
        string lngStr;
        string valStr;
        getline(lineStream, latStr, ',');
        getline(lineStream, lngStr, ',');
        getline(lineStream, valStr, ',');

        data.emplace();
    }
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( "GLUT" );

    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutPassiveMotionFunc( passiveMotion );
    //glutMouseWheelFunc( wheel );
    glutDisplayFunc( display );

    loadFile(argv[1]);

    glutMainLoop();
    return 0;
}
