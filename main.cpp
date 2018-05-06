#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

#include <iostream>
using namespace std;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


glm::dvec3 Unproject( const glm::dvec3& win )
{
    glm::ivec4 view;
    glm::dmat4 proj, model;
    glGetDoublev( GL_MODELVIEW_MATRIX, &model[0][0] );
    glGetDoublev( GL_PROJECTION_MATRIX, &proj[0][0] );
    glGetIntegerv( GL_VIEWPORT, &view[0] );

    glm::dvec3 world = glm::unProject( win, model, proj, view );
    return world;
}

// unprojects the given window point
// and finds the ray intersection with the Z=0 plane
glm::dvec2 PlaneUnproject( const glm::dvec2& win )
{
    glm::dvec3 world1 = Unproject( glm::dvec3( win, 0.01 ) );
    glm::dvec3 world2 = Unproject( glm::dvec3( win, 0.99 ) );

    // u is a value such that:
    // 0 = world1.z + u * ( world2.z - world1.z )
    double u = -world1.z / ( world2.z - world1.z );
    // clamp u to reasonable values
    if( u < 0 ) u = 0;
    if( u > 1 ) u = 1;

    return glm::dvec2( world1 + u * ( world2 - world1 ) );
}

// pixels per unit
const double ppu = 1.0;

glm::dvec2 center( 0 );
double scale = 1.0;
void ApplyCamera()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    const double w = glutGet( GLUT_WINDOW_WIDTH ) / ppu;
    const double h = glutGet( GLUT_WINDOW_HEIGHT ) / ppu;
    glOrtho( -w/2, w/2, -h/2, h/2, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glScaled( scale, scale, 1.0 );
    glTranslated( -center[0], -center[1], 0 );
}

glm::dvec2 mPos;

glm::dvec2 centerStart( 0 );
int btn = -1;

void mouse( int button, int state, int x, int y )
{
    ApplyCamera();

    y = glutGet( GLUT_WINDOW_HEIGHT ) - y;
    mPos = glm::ivec2( x, y );

    btn = button;
    if( GLUT_LEFT_BUTTON == btn && GLUT_DOWN == state )
    {
        centerStart = PlaneUnproject( glm::dvec2( x, y ) );
    }
    if( GLUT_LEFT_BUTTON == btn && GLUT_UP == state )
    {
        btn = -1;
    }

    glutPostRedisplay();
}

void motion( int x, int y )
{
    y = glutGet( GLUT_WINDOW_HEIGHT ) - y;
    mPos = glm::ivec2( x, y );

    if( GLUT_LEFT_BUTTON == btn )
    {
        ApplyCamera();
        glm::dvec2 cur = PlaneUnproject( glm::dvec2( x, y ) );
        center += ( centerStart - cur );
    }

    glutPostRedisplay();
}

void passiveMotion( int x, int y )
{
    y = glutGet( GLUT_WINDOW_HEIGHT ) - y;
    mPos = glm::ivec2( x, y );
    glutPostRedisplay();
}

void wheel( int wheel, int direction, int x, int y )
{
    y = glutGet( GLUT_WINDOW_HEIGHT ) - y;
    mPos = glm::ivec2( x, y );

    ApplyCamera();
    glm::dvec2 beforeZoom = PlaneUnproject( glm::dvec2( x, y ) );

    const double scaleFactor = 0.90;
    if( direction == -1 )   scale *= scaleFactor;
    if( direction ==  1 )   scale /= scaleFactor;

    ApplyCamera();
    glm::dvec2 afterZoom = PlaneUnproject( glm::dvec2( x, y ) );

    center += ( beforeZoom - afterZoom );

    glutPostRedisplay();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    ApplyCamera();

    glm::dvec2 cur = PlaneUnproject( mPos );
    cout << cur.x << " " << cur.y << " " << scale << endl;

    glPushMatrix();
    glScalef( 50, 50, 1 );
    glBegin( GL_QUADS );
    glColor3ub( 255, 255, 255 );
    glVertex2i( -1, -1 );
    glVertex2i(  1, -1 );
    glVertex2i(  1,  1 );
    glVertex2i( -1,  1 );
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "GLUT" );

    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    //glutMouseWheelFunc( wheel );
    glutDisplayFunc( display );
    glutPassiveMotionFunc( passiveMotion );

    glutMainLoop();
    return 0;
}
