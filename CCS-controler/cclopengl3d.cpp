#include "cclopengl3d.h"
#include "widget.h"

#include <GL/glu.h>

cclOpenGL3D::cclOpenGL3D(QWidget *parent)
    : QOpenGLWidget(parent)
{
    follow = true;
}

void cclOpenGL3D::setPulleyAngle(GLfloat _angle)
{
    pulleyAngle = _angle;
}

void cclOpenGL3D::setVehiclePosition(GLfloat _position)
{
    vehiclePosition = _position;
}

void cclOpenGL3D::animate()
{
    update();
    wheelRotateAngle = vehiclePosition*25*10;
    finalPulleyAngle = pulleyAngle*180;
    if(finalPulleyAngle > 75){
        finalPulleyAngle = 75;
    }else if(finalPulleyAngle < -75){
        finalPulleyAngle = -75;
    }
}

void cclOpenGL3D::followEnable(bool _follow)
{
    follow = _follow;
}

void cclOpenGL3D::leftRotate()
{
    horizontalRotateAngle+=5;
}

void cclOpenGL3D::rightRotate()
{
    horizontalRotateAngle-=5;
}

void cclOpenGL3D::upRotate()
{
    verticalRotateAngle+=5;
}

void cclOpenGL3D::downRotate()
{
    verticalRotateAngle-=5;
}

void cclOpenGL3D::resetRotate()
{
    horizontalRotateAngle=0;
    verticalRotateAngle=0;
}


void cclOpenGL3D::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    wheelRadius = 0.35;
    pulleyRadius = 0.30;
}

void cclOpenGL3D::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void cclOpenGL3D::drawFilledCircle(float z0, float radius)
{
    float alpha;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.0, 0.0, z0);
    for(int count=0; count <= 100; count++){
        alpha = count*2*3.1415926/100;
        if((count<5) || ((count>=25)&&(count<30))  || ((count>=50)&&(count<55)) || ((count>=75)&&(count<80))){
            glColor3f(0.0, 1.0, 1.0);
        }else{
            glColor3f(1.0, 1.0, 1.0);
        }
        glVertex3f(radius*cos(alpha), radius*sin(alpha), z0);
    }
    glEnd();
}

void cclOpenGL3D::drawCuboid(float x0, float y0, float z0)
{
    glBegin(GL_QUADS);
    //up
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y0, -z0);
    glVertex3f(-x0, y0, -z0);
    glVertex3f(-x0, y0, z0);
    //down
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(x0, -y0, z0);
    glVertex3f(x0, -y0, -z0);
    glVertex3f(-x0, -y0, -z0);
    glVertex3f(-x0, -y0, z0);
    //front
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(x0, y0, z0);
    glVertex3f(-x0, y0, z0);
    glVertex3f(-x0, -y0, z0);
    glVertex3f(x0, -y0, z0);

    //back
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(x0, y0, -z0);
    glVertex3f(-x0, y0, -z0);
    glVertex3f(-x0, -y0, -z0);
    glVertex3f(x0, -y0, -z0);

    //right
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, -y0, z0);
    glVertex3f(x0, -y0, -z0);
    glVertex3f(x0, y0, -z0);

    //left
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(-x0, y0, z0);
    glVertex3f(-x0, -y0, z0);
    glVertex3f(-x0, -y0, -z0);
    glVertex3f(-x0, y0, -z0);

    glEnd();
}

void cclOpenGL3D::drawCylinder(float z1, float z2, float radius)
{
    float alpha;
    glBegin(GL_QUAD_STRIP);
    for(int count=0;count<=100;count++){
        alpha = count*2*3.1415926/100;
        glVertex3f(radius*cos(alpha), radius*sin(alpha), z1);
        glVertex3f(radius*cos(alpha), radius*sin(alpha), z2);
    }
    glEnd();
}

void cclOpenGL3D::drawWheels(float z1, float z2, float radius)
{
    drawFilledCircle(z1, radius);
    drawCylinder(z1, z2, radius);
    drawFilledCircle(z2, radius);
    drawFilledCircle(-z1, radius);
    drawCylinder(-z1, -z2, radius);
    drawFilledCircle(-z2, radius);
}

void cclOpenGL3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // horizantal adaptation
    glTranslatef(0.0, 0.0, -8.0);
    glRotatef(horizontalRotateAngle, 0.0, 1.0, 0.0);

    if(!follow){
        // movement
        glTranslatef(wheelRotateAngle/10, 0.0, 0.0);
    }

    glTranslatef(0.0, -1.5, -0.0);
    glRotatef(verticalRotateAngle, 1.0, 0.0, 0.0);

    // earth
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(100.0, -0.65, 0.5);
    glVertex3f(100.0, -0.65, -0.5);
    glVertex3f(-100.0, -0.65, -0.5);
    glVertex3f(-100.0, -0.65, 0.5);
    glEnd();

    // start point
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-wheelRotateAngle/10+0.2, -0.64, 0.5);
    glVertex3f(-wheelRotateAngle/10+0.2, -0.64, -0.5);
    glVertex3f(-wheelRotateAngle/10-0.2, -0.64, -0.5);
    glVertex3f(-wheelRotateAngle/10-0.2, -0.64, 0.5);
    glEnd();

    // body
    drawCuboid(2.0, 0.3, 0.3);

    // pulley
    glPushMatrix();
    glTranslatef(0.0, 0.3, 0.0);
    glRotatef(finalPulleyAngle, 0.0, 0.0, -1.0);
    drawFilledCircle(0.17, pulleyRadius);
    drawCylinder(0.17, -0.17, pulleyRadius);
    drawFilledCircle(-0.17, pulleyRadius);
    glTranslatef(0.0, 1.8, 0.0);
    drawCuboid(0.15, 2.1, 0.15);
    glPopMatrix();

    // right wheels
    glPushMatrix();
    glTranslatef(1.0, -0.3, 0);
    glRotatef(wheelRotateAngle*10, 0.0, 0.0, -1.0);
    drawWheels(0.25, 0.35, 0.35);
    glPopMatrix();

    // left wheels
    glPushMatrix();
    glTranslatef(-1.0, -0.3, 0);
    glRotatef(wheelRotateAngle*10, 0.0, 0.0, -1.0);
    drawWheels(0.25, 0.35, 0.35);
    glPopMatrix();

}
