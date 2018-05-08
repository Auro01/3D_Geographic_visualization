#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

#include <iostream>
using namespace std;
#include "BmpLoader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



unsigned int ID;
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { -5.0f, 5.0f, 19.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


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
    if ((button == 3) || (button == 4)) // It's a wheel event
   {
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
       y = glutGet( GLUT_WINDOW_HEIGHT ) - y;
    mPos = glm::ivec2( x, y );

    ApplyCamera();
    glm::dvec2 beforeZoom = PlaneUnproject( glm::dvec2( x, y ) );

    const double scaleFactor = 0.90;
    if( button == 3 )   scale /= scaleFactor;
    if( button == 4 )   scale *= scaleFactor;

    ApplyCamera();
    glm::dvec2 afterZoom = PlaneUnproject( glm::dvec2( x, y ) );

    center += ( beforeZoom - afterZoom );

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

void display()
{
    glEnable(GL_TEXTURE_2D);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    ApplyCamera();

    glPushMatrix();
    glScalef( 250, 250, 1 );
    glBegin( GL_QUADS );
    glColor3ub( 255, 255, 255 );
     glTexCoord2f(1.0,1.0);
                glVertex3f(1.0,1.0,0.0);
                glTexCoord2f(0.0,1.0); glVertex3f(-1.0,1.0,0.0);
                glTexCoord2f(0.0,0.0); glVertex3f(-1.0,-1.0,0.0);
                glTexCoord2f(1.0,0.0); glVertex3f(1.0,-1.0,0.0);
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);
}

void init(){
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
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "GLUT" );
    LoadTexture("map.bmp");

    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    //glutMouseWheelFunc( wheel );
    glutDisplayFunc( display );
    glutPassiveMotionFunc( passiveMotion );

    init();

    glutMainLoop();
    return 0;
}
