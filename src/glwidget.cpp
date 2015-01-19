#include "glwidget.h"
#include "Tekstura.h"
#include "NebeskoTelo.h"

#include <string>
#include <cstdio>

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
#include <QFont>

// test
#include <iostream>
using std::cout;
using std::endl;
//

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::unique_ptr;
using std::get;
using std::make_unique;
using std::to_string;
using std::string;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    prethodno_vreme = system_clock::time_point::min();

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

    pozadina = make_unique<NebeskoTelo>("", UNIV_R, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                        make_unique<Tekstura>(confDir + background), true);

    const QString STARS = "stars";
    const QString PLANETS = "planets";
    const QString NAME = "name";
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

    QJsonArray starsArray = system[STARS].toArray();
    for(const QJsonValue &starValue: starsArray)
    {
        QJsonObject star = starValue.toObject();
        stars.push_back(make_unique<NebeskoTelo>(
                            star[NAME].toString().toStdString(),
                            star[RADIUS].toDouble(),
                            star[REVOLUTION_VELOCITY].toDouble(),
                            star[REVOLUTION_RADIUS].toDouble(),
                            star[ROTATION_VELOCITY].toDouble(),
                            star[SLOPE].toDouble(),
                            star[ROTATION_SLOPE].toDouble(),
                            50,
                            make_unique<Tekstura>(confDir + star[TEXTURE].toString()),
                            true
                            )
                        );
    }

    QJsonArray planetsArray = system[PLANETS].toArray();
    for(const QJsonValue &planetValue: planetsArray)
    {
        QJsonObject planet = planetValue.toObject();
        auto new_planet = make_unique<NebeskoTelo>(
                            planet[NAME].toString().toStdString(),
                            planet[RADIUS].toDouble(),
                            planet[REVOLUTION_VELOCITY].toDouble(),
                            planet[REVOLUTION_RADIUS].toDouble(),
                            planet[ROTATION_VELOCITY].toDouble(),
                            planet[SLOPE].toDouble(),
                            planet[ROTATION_SLOPE].toDouble(),
                            200,
                            make_unique<Tekstura>(confDir + planet[TEXTURE].toString())
                          );

        if(!planet[RING].isNull())
        {
            QJsonObject ring = planet[RING].toObject();
            new_planet->dodajPrsten(make_unique<Prsten>(
                                        ring[RING_INNER_RADIUS].toDouble(),
                                        ring[RING_OUTTER_RADIUS].toDouble(),
                                        make_unique<Tekstura>(confDir + ring[TEXTURE].toString()),
                                        10
                                        )
                                    );
        }

        if(!planet[SATELLITES].isNull())
        {
            QJsonArray satellites = planet[SATELLITES].toArray();
            for(const QJsonValue &satelliteValue: satellites)
            {
                QJsonObject satellite = satelliteValue.toObject();
                new_planet->dodajSatelit(make_unique<NebeskoTelo>(
                                             satellite[NAME].toString().toStdString(),
                                             satellite[RADIUS].toDouble(),
                                             satellite[REVOLUTION_VELOCITY].toDouble(),
                                             satellite[REVOLUTION_RADIUS].toDouble(),
                                             satellite[ROTATION_VELOCITY].toDouble(),
                                             satellite[SLOPE].toDouble(),
                                             satellite[ROTATION_SLOPE].toDouble(),
                                             50,
                                             make_unique<Tekstura>(
                                                 confDir + satellite[TEXTURE].toString()
                                             )
                                             )
                                         );
            }
        }

        planets.push_back( std::move(new_planet) );
    }
}

std::string GLWidget::view_to_string(view_modes mode)
{
    switch (mode) {
    case view_modes::AXIS:   return "AXIS"; break;
    case view_modes::SPHERE: return "SPHERE"; break;
    case view_modes::CENTER: return "CENTER"; break;
    default:                 return ""; break;
    }
}

bool GLWidget::isInitialise() const
{
    return (pozadina && !stars.empty() && !planets.empty());
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand(time(nullptr));
}

void GLWidget::paintGL()
{
    GLfloat PozicijaSvetla[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(!isInitialise())
        return;

    // looking-at body
    double view_x;
    double view_z;

    if(pogled == 0)
    {
        view_x = 0;
        view_z = 0;
    }
    else
    {
        auto temp = planets.at(pogled - 1)->getPos();
        view_x = get<0>(temp);
        view_z = get<1>(temp);
    }

    // referent body
    double pos_x;
    double pos_z;

    if(planeta != -1)
    {
        auto temp = planets.at(planeta)->getPos();
        pos_x = get<0>(temp);
        pos_z = get<1>(temp);
    }
    else
    {
        pos_x = -70;
        pos_z = 30;
    }

    // view modes
    double pos_y;
    int x_diff;
    int y_diff;
    int z_diff;
    int r_diff;

    switch (view_mode)
    {
    case view_modes::AXIS:
        pos_y = scale_y * 0.98 * UNIV_R;
        gluLookAt(pos_x, pos_y, pos_z, view_x, 0, view_z, 0, 1, 0);
        break;

    case view_modes::SPHERE:
        //TODO: need refining
        pos_y = view_radius * UNIV_R;
        y_diff = cos(RAD_PER_DEG*scale_y*360) * view_radius * UNIV_R;
        r_diff = sin(RAD_PER_DEG*scale_y*360) * view_radius * UNIV_R;
        x_diff = sin(RAD_PER_DEG*scale_x*360) * r_diff;
        z_diff = cos(RAD_PER_DEG*scale_x*360) * r_diff;
        gluLookAt(pos_x + x_diff, y_diff, pos_z + z_diff, view_x, 0, view_z, 0, 1, 0);
        break;

    case view_modes::CENTER:
        gluLookAt(pos_x, 0.0, pos_z, view_x, 0, view_z, 0, 1, 0);
        break;

    default:
        // default to AXIS
        pos_y = scale_y * 0.98 * UNIV_R;
        gluLookAt(pos_x, pos_y, pos_z, view_x, 0, view_z, 0, 1, 0);
        break;
    }

    glLightfv(GL_LIGHT1, GL_POSITION, PozicijaSvetla);

    // advancing time
    milliseconds proteklo = milliseconds::zero();
    if( prethodno_vreme != system_clock::time_point::min() )
        proteklo = duration_cast<milliseconds>(system_clock::now() - prethodno_vreme);
    else
        prethodno_vreme = system_clock::now();

    pozadina->crtaj(this);

    for(const auto &star: stars)
    {
        star->crtaj(this);
        star->pomeri(proteklo.count());
    }

    for(unsigned i = 0; i < planets.size(); i++)
    {
        const auto &planet = planets[i];
        if (view_mode != view_modes::CENTER || planeta != (int)i)
        {
            planet->crtaj(this);
        }
        planet->pomeri(proteklo.count());
    }

    prethodno_vreme += proteklo;

    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60, (GLfloat)win_width/(GLfloat)win_height, 1, 1200);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    qglColor(Qt::white);

    renderText(-2.1, 0.60, 0.0,
               QString::fromStdString("View mode: " + view_to_string(view_mode)),
               QFont("Arial", 12, QFont::Bold));

    renderText(-2.1, 0.55, 0.0,
               QString::fromStdString("Referent body: " + (planeta==-1? "Nowhere": planets[planeta]->ime)),
               QFont("Arial", 12, QFont::Bold));

    renderText(-2.1, 0.50, 0.0,
               QString::fromStdString("View body: " + planets[pogled]->ime),
               QFont("Arial", 12, QFont::Bold));

    if (view_mode == view_modes::SPHERE)
    {
        char rad_str[5];
        snprintf(rad_str, 5, "%.2f", view_radius);
        renderText(-2.1, 0.45, 0.0,
                   QString::fromStdString("View radius: " + string(rad_str)),
                   QFont("Arial", 12, QFont::Bold));
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();

//    glFinish();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLint) width, (GLint) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)width/(GLfloat)height, 1, 1200);

    glMatrixMode(GL_MODELVIEW);

    win_width = width;
    win_height = height;
}

void GLWidget::advanceTime()
{
    updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isInitialise())
        return;

    int x = event->x();
    int y = event->y();

    scale_x = ((float)(x - win_width/2))/win_width;
    scale_y = ((float)(y - win_height/2))/win_height;
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

    case Qt::Key_Up:
        if(planeta < (int)planets.size() - 1)
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
        if (pogled < (int)planets.size())
            pogled++;
        break;

    case Qt::Key_Z:
        if (view_radius >= 0.1)
        {
            view_radius -= 0.05;
        }
        break;

    case Qt::Key_X:
        if (view_radius <= 0.9)
        {
            view_radius += 0.05;
        }
        break;

    case Qt::Key_1:
        view_mode = view_modes::AXIS;
        break;

    case Qt::Key_2:
        view_mode = view_modes::SPHERE;
        break;

    case Qt::Key_3:
        view_mode = view_modes::CENTER;
        break;

    default:
        break;
    }
}
