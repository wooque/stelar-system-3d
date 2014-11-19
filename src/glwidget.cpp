#include "glwidget.h"

#include "Tekstura.h"
#include "NebeskoTelo.h"

#include <cmath>
#include <chrono>

#include <GL/gl.h>
#include <GL/glu.h>

#include <QTimer>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>

// test
#include <iostream>

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::unique_ptr;
using std::get;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(advanceTime()));
    timer->start(20);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    makeCurrent();
}

void GLWidget::loadConfiguration(QString filename)
{
    QString settings;
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    settings = file.readAll();
    file.close();

    QJsonDocument systemJson = QJsonDocument::fromJson(settings.toUtf8());
    QJsonObject system = systemJson.object();
    QString background = system["background"].toString();

    QFileInfo confInfo(file);
    QString confDir = confInfo.absolutePath() + QDir::separator();

    pozadina = unique_ptr<NebeskoTelo>(new NebeskoTelo(500.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                                       unique_ptr<Tekstura>(new Tekstura(confDir + background)), true));

    const QString STARS = "stars";
    const QString PLANETS = "planets";
    const QString RADIUS = "radius";
    const QString REVOLUTION_VELOCITY = "revolution_velocity";
    const QString REVOLUTION_RADIUS = "revolution_radius";
    const QString ROTATION_VELOCITY = "rotation_velocity";
    const QString SLOPE = "slope";
    const QString ROTATION_SLOPE = "rotation_slope";
    const QString TEXTURE = "texture";
    const QString RING = "ring";
    const QString RING_INNER_RADIUS = "inner_radius";
    const QString RING_OUTTER_RADIUS = "outter_radius";
    const QString SATELLITES = "satellites";

    QJsonArray stars = system[STARS].toArray();
    for(const QJsonValue &starValue: stars)
    {
        QJsonObject star = starValue.toObject();
        Sunce = unique_ptr<NebeskoTelo>(new NebeskoTelo(star[RADIUS].toDouble(),
                                                        star[REVOLUTION_VELOCITY].toDouble(),
                                                        star[REVOLUTION_RADIUS].toDouble(),
                                                        star[ROTATION_VELOCITY].toDouble(),
                                                        star[SLOPE].toDouble(),
                                                        star[ROTATION_SLOPE].toDouble(),
                                                        0,
                                                        unique_ptr<Tekstura>(new Tekstura(confDir + star[TEXTURE].toString())),
                                                        true));
        QJsonArray planets = system[PLANETS].toArray();
        for(const QJsonValue &planetValue: planets)
        {
            QJsonObject planet = planetValue.toObject();
            auto new_planet = unique_ptr<NebeskoTelo>(new NebeskoTelo(planet[RADIUS].toDouble(),
                                                                      planet[REVOLUTION_VELOCITY].toDouble(),
                                                                      planet[REVOLUTION_RADIUS].toDouble(),
                                                                      planet[ROTATION_VELOCITY].toDouble(),
                                                                      planet[SLOPE].toDouble(),
                                                                      planet[ROTATION_SLOPE].toDouble(),
                                                                      200,
                                                                      unique_ptr<Tekstura>(new Tekstura(confDir + planet[TEXTURE].toString()))));
            if(!planet[RING].isNull())
            {
                QJsonObject ring = planet[RING].toObject();
                new_planet->dodajPrsten(unique_ptr<Prsten>(new Prsten(ring[RING_INNER_RADIUS].toDouble(),
                                                                      ring[RING_OUTTER_RADIUS].toDouble(),
                                                                      unique_ptr<Tekstura>(new Tekstura(confDir + ring[TEXTURE].toString())),
                                                                      10)));
            }

            if(!planet[SATELLITES].isNull())
            {
                QJsonArray satellites = planet[SATELLITES].toArray();
                for(const QJsonValue &satelliteValue: satellites)
                {
                    QJsonObject satellite = satelliteValue.toObject();
                    new_planet->dodajSatelit(unique_ptr<NebeskoTelo>(new NebeskoTelo(satellite[RADIUS].toDouble(),
                                                                                     satellite[REVOLUTION_VELOCITY].toDouble(),
                                                                                     satellite[REVOLUTION_RADIUS].toDouble(),
                                                                                     satellite[ROTATION_VELOCITY].toDouble(),
                                                                                     satellite[SLOPE].toDouble(),
                                                                                     satellite[ROTATION_SLOPE].toDouble(),
                                                                                     200,
                                                                                     unique_ptr<Tekstura>(new Tekstura(confDir + satellite[TEXTURE].toString())))));
                }
            }

            Sunce->dodajSatelit( std::move(new_planet) );
        }
    }
}

bool GLWidget::isInitialise() const
{
    return (Sunce && pozadina);
}
void GLWidget::initializeGL()
{
    GLfloat AmbijentalnoSvetlo[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat DifuznoSvetlo[] =      { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT1, GL_AMBIENT, AmbijentalnoSvetlo);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, DifuznoSvetlo);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbijentalnoSvetlo);

    glEnable(GL_LIGHT1);

    srand(time(nullptr));
}

void GLWidget::paintGL()
{
    GLfloat PozicijaSvetla[] =     { 0.0f, 0.0f, 0.0f, 1.0f };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(!isInitialise())
        return;

    int view_x;
    int view_z;
    if(pogled == 0)
    {
        view_x = 0;
        view_z = 0;
    }
    else
    {
        auto temp = Sunce->getSatelitPos(pogled - 1);
        view_x = get<0>(temp);
        view_z = get<1>(temp);
    }

    int pos_x;
    int pos_z;
    if(planeta != -1)
    {
        auto temp = Sunce->getSatelitPos(planeta);
        pos_x = get<0>(temp);
        pos_z = get<1>(temp);
    }
    else
    {
        pos_x = -70;
        pos_z = 30;
    }
    gluLookAt(pos_x, visina, pos_z, view_x, 0, view_z, 0, 1, 0);

    glLightfv(GL_LIGHT1, GL_POSITION, PozicijaSvetla);

    long proteklo = 0;
    if( prethodno_vreme != 0 )
        proteklo = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - prethodno_vreme;
    else
        prethodno_vreme = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    pozadina->crtaj();
    Sunce->crtaj();
    Sunce->pomeri(proteklo);

    prethodno_vreme += proteklo;

//    glFinish();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLint) width, (GLint) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)width/(GLfloat)height, 1, 1200);

    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::advanceTime()
{
    updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isInitialise())
        return;

    int y = event->y();

    if(prethodno_y == -1)
    {
        prethodno_y = y;
        return;
    }

    int temp = y - prethodno_y;

    if(temp > 0)
        visina++;
    if(temp < 0)
        visina--;

    prethodno_y = y;
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (!isInitialise())
        return;

    switch (event->key())
    {
    case Qt::Key_Escape:
        QApplication::quit();
        break;

    case Qt::Key_Up :
        if(planeta < Sunce->broj_satelita() - 1)
            planeta++;
        break;

    case Qt::Key_Down:
        if(planeta >= 0)
            planeta --;
        break;

    case Qt::Key_Left:
        if (pogled > 0)
            pogled--;
        break;

    case Qt::Key_Right:
        if (pogled < Sunce->broj_satelita())
            pogled++;
        break;
    }
}
