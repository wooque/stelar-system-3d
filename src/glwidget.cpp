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

const QString GLWidget::BACKGROUND = "background";
const QString GLWidget::LIGHTING = "lighting";
const QString GLWidget::AMBIENT = "ambient";
const QString GLWidget::DIFUSE = "difuse";
const QString GLWidget::POSITION = "position";
const QString GLWidget::STARS = "stars";
const QString GLWidget::PLANETS = "planets";
const QString GLWidget::NAME = "name";
const QString GLWidget::RADIUS = "radius";
const QString GLWidget::REVOLUTION_VELOCITY = "revolution_velocity";
const QString GLWidget::REVOLUTION_RADIUS = "revolution_radius";
const QString GLWidget::ROTATION_VELOCITY = "rotation_velocity";
const QString GLWidget::SLOPE = "slope";
const QString GLWidget::ROTATION_SLOPE = "rotation_slope";
const QString GLWidget::TEXTURE = "texture";
const QString GLWidget::RING = "ring";
const QString GLWidget::RING_INNER_RADIUS = "inner_radius";
const QString GLWidget::RING_OUTTER_RADIUS = "outter_radius";
const QString GLWidget::SATELLITES = "satellites";

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

void GLWidget::loadLightingData(float *lighting, const QJsonObject &data, const QString &type)
{
    QJsonArray lightingArray = data[type].toArray();
    int i = 0;
    for(const QJsonValue &lightingComponent: lightingArray)
    {
        float component = lightingComponent.toDouble();
        lighting[i++] = component;
    }
}

unique_ptr<NebeskoTelo> GLWidget::loadStelarBody(const QString &conf_dir,
                                                 const QJsonObject &data,
                                                 bool is_star,
                                                 int trail_length)
{
    return make_unique<NebeskoTelo>(
                data[NAME].toString().toStdString(),
                data[RADIUS].toDouble(),
                data[REVOLUTION_VELOCITY].toDouble(),
                data[REVOLUTION_RADIUS].toDouble(),
                data[ROTATION_VELOCITY].toDouble(),
                data[SLOPE].toDouble(),
                data[ROTATION_SLOPE].toDouble(),
                trail_length,
                make_unique<Tekstura>(conf_dir + data[TEXTURE].toString()),
                is_star
                );
}

void GLWidget::loadConfiguration(QString filename)
{
    QString settings;
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    settings = file.readAll();
    file.close();

    QFileInfo confInfo(file);
    QString confDir = confInfo.absolutePath() + QDir::separator();

    QJsonDocument systemJson = QJsonDocument::fromJson(settings.toUtf8());
    QJsonObject system = systemJson.object();

    QJsonObject lightingData = system[LIGHTING].toObject();
    loadLightingData(ambient, lightingData, AMBIENT);
    loadLightingData(difuse, lightingData, DIFUSE);
    loadLightingData(position, lightingData, POSITION);

    QString background = system[BACKGROUND].toString();
    pozadina = make_unique<NebeskoTelo>("", 2*UNIV_R, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                        make_unique<Tekstura>(confDir + background), true);

    QJsonArray starsArray = system[STARS].toArray();
    for(const QJsonValue &starValue: starsArray)
    {
        QJsonObject star = starValue.toObject();
        stars.push_back(loadStelarBody(confDir, star, true, 50));
    }

    QJsonArray planetsArray = system[PLANETS].toArray();
    for(const QJsonValue &planetValue: planetsArray)
    {
        QJsonObject planet = planetValue.toObject();
        auto new_planet = loadStelarBody(confDir, planet, false, 200);

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
                new_planet->dodajSatelit(loadStelarBody(confDir, satellite, false, 50));
            }
        }

        planets.push_back( std::move(new_planet) );
    }
    initializeGL();
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
    GLfloat AmbijentalnoSvetlo[] = { ambient[0], ambient[1], ambient[2], 1.0f };
    GLfloat DifuznoSvetlo[] =      { difuse[0], difuse[1], difuse[2], 1.0f };
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
    GLfloat PozicijaSvetla[] = {position[0], position[1], position[2], 1.0f };

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
    int r, r2;

    switch (view_mode)
    {
    case view_modes::AXIS:
        pos_y = scale_y * 0.98 * UNIV_R;
        gluLookAt(pos_x, pos_y, pos_z, view_x, 0, view_z, 0, 1, 0);
        break;

    case view_modes::SPHERE:
        r = view_radius * UNIV_R;
        y_diff = sin(RAD_PER_DEG*scale_y*360) * r;
        r2 = cos(RAD_PER_DEG*scale_y*360) * r;
        x_diff = sin(RAD_PER_DEG*scale_x*360) * r2;
        z_diff = cos(RAD_PER_DEG*scale_x*360) * r2;
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
    gluPerspective(60, (GLfloat)win_width/(GLfloat)win_height, 1, 2000);
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
    gluPerspective(60, (GLfloat)width/(GLfloat)height, 1, 2000);

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
