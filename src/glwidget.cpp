#include "glwidget.h"

#include <QTimer>
#include <math.h>

// test
#include <OpenGL_planete.h>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    init_sistem();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(advanceTime()));
    timer->start(20);
}

GLWidget::~GLWidget()
{
    makeCurrent();
}

void GLWidget::initializeGL()
{
    // test
//    static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };

//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_DEPTH_TEST);

//    glEnable(GL_NORMALIZE);
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    init_GL();
}

void GLWidget::paintGL()
{
    // test
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glPushMatrix();

//    glPopMatrix();
    draw();
}

void GLWidget::resizeGL(int width, int height)
{
    // test
//    int side = qMin(width, height);
//    glViewport((width - side) / 2, (height - side) / 2, side, side);

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslated(0.0, 0.0, -40.0);
    reshape(width, height);
}

void GLWidget::advanceTime()
{
    updateGL();
}
