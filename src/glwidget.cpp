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
using std::to_string;
using std::string;

const QString GLWidget::BACKGROUND = "background";
const QString GLWidget::AMBIENT_LIGHTING = "ambient-lighting";
const QString GLWidget::STAR_LIGHTING = "lighting";
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

void GLWidget::loadLightingData(float *lighting, const QJsonValue &data)
{
    QJsonArray lightingArray = data.toArray();
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
    return unique_ptr<NebeskoTelo>(
                new NebeskoTelo(
                    data[NAME].toString().toStdString(),
                    data[RADIUS].toDouble(),
                    data[REVOLUTION_VELOCITY].toDouble(),
                    data[REVOLUTION_RADIUS].toDouble(),
                    data[ROTATION_VELOCITY].toDouble(),
                    data[ROTATION_SLOPE].toDouble(),
                    data[SLOPE].toDouble(),
                    trail_length,
                    unique_ptr<Tekstura>(new Tekstura(conf_dir + data[TEXTURE].toString())),
                    is_star));
}

void GLWidget::loadConfiguration(QString filename)
{
    // clear current state
    bodies.clear();
    view_body = 0;
    ref_body = -1;
    view_radius = 0.5;
    view_mode = view_modes::AXIS;
    star_num = 0;

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

    loadLightingData(ambient, system[AMBIENT_LIGHTING]);

    QString background = system[BACKGROUND].toString();
    pozadina = unique_ptr<NebeskoTelo>(
                new NebeskoTelo("", 2*UNIV_R, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                unique_ptr<Tekstura>(new Tekstura(confDir + background)),
                                true));

    QJsonArray starsArray = system[STARS].toArray();
    int i = 0;
    for(const QJsonValue &starValue: starsArray)
    {
        QJsonObject star = starValue.toObject();
        auto new_star = loadStelarBody(confDir, star, true, 50);

        if (!star[STAR_LIGHTING].isNull() && i < 8)
        {
            float star_light[3];
            loadLightingData(star_light, star[STAR_LIGHTING]);
            new_star->dodajSvetlo(
                        unique_ptr<Svetlo>(new Svetlo(i++,star_light[0],star_light[1],star_light[2])));
        }
        bodies.push_back(std::move(new_star));
    }
    star_num = i;

    QJsonArray planetsArray = system[PLANETS].toArray();
    for(const QJsonValue &planetValue: planetsArray)
    {
        QJsonObject planet = planetValue.toObject();
        auto new_planet = loadStelarBody(confDir, planet, false, 200);

        if(!planet[RING].isNull())
        {
            QJsonObject ring = planet[RING].toObject();
            new_planet->dodajPrsten(unique_ptr<Prsten>(
                                        new Prsten(
                                            ring[RING_INNER_RADIUS].toDouble(),
                                            ring[RING_OUTTER_RADIUS].toDouble(),
                                            unique_ptr<Tekstura>(
                                                new Tekstura(confDir + ring[TEXTURE].toString())),
                                            10)));
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

        bodies.push_back( std::move(new_planet) );
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
    return (pozadina && !bodies.empty());
}

void GLWidget::initializeGL()
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    GLfloat AmbijentalnoSvetlo[] = { ambient[0], ambient[1], ambient[2], 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbijentalnoSvetlo);

    for(int i = 0; i < star_num; i++)
    {
        glLightfv(Svetlo::LIGHTS[i], GL_AMBIENT, AmbijentalnoSvetlo);
        glEnable(Svetlo::LIGHTS[i]);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand(time(nullptr));
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(!isInitialise())
        return;

    // looking-at body
    double view_x;
    double view_y;
    double view_z;

    if(view_body == -1)
    {
        view_x = 0;
        view_y = 0;
        view_z = 0;
    }
    else
    {
        auto temp = bodies.at(view_body)->getPos();
        view_x = get<0>(temp);
        view_y = get<1>(temp);
        view_z = get<2>(temp);
    }

    // referent body
    double pos_x;
    double pos_y;
    double pos_z;

    if(ref_body != -1)
    {
        auto temp = bodies.at(ref_body)->getPos();
        pos_x = get<0>(temp);
        pos_y = get<1>(temp);
        pos_z = get<2>(temp);
    }
    else
    {
        pos_x = 0;
        pos_y = 0;
        pos_z = 0;
    }

    // view modes
    int x_diff;
    int y_diff;
    int z_diff;
    int r, r2;

    switch (view_mode)
    {
    case view_modes::AXIS:
        y_diff = scale_y * 0.98 * UNIV_R;
        gluLookAt(pos_x, pos_y + y_diff, pos_z, view_x, view_y, view_z, 0, 1, 0);
        break;

    case view_modes::SPHERE:
        r = view_radius * UNIV_R;
        y_diff = -sin(RAD_PER_DEG*scale_y*360) * r;
        r2 = cos(RAD_PER_DEG*scale_y*360) * r;
        x_diff = -sin(RAD_PER_DEG*scale_x*360) * r2;
        z_diff = cos(RAD_PER_DEG*scale_x*360) * r2;
        gluLookAt(pos_x + x_diff, pos_y + y_diff, pos_z + z_diff, view_x, view_y, view_z, 0, 1, 0);
        break;

    case view_modes::CENTER:
        gluLookAt(pos_x, pos_y, pos_z, view_x, view_y, view_z, 0, 1, 0);
        break;

    default:
        break;
    }

    // advancing time
    milliseconds proteklo = milliseconds::zero();
    if( prethodno_vreme != system_clock::time_point::min() )
        proteklo = duration_cast<milliseconds>(system_clock::now() - prethodno_vreme);
    else
        prethodno_vreme = system_clock::now();

    for (const auto &star: bodies)
    {
        star->crtajSvetlo();
    }

    pozadina->crtaj(this);

    for(unsigned i = 0; i < bodies.size(); i++)
    {
        const auto &body = bodies[i];
        if (view_mode != view_modes::CENTER || ref_body != (int)i)
        {
            body->crtaj(this);
        }
        body->pomeri(proteklo.count());
    }

    prethodno_vreme += proteklo;

    glPushMatrix();
    qglColor(Qt::white);

    renderText(10, 20,
               QString::fromStdString("View mode: " + view_to_string(view_mode)),
               QFont("Arial", 12, QFont::Bold));

    renderText(10, 40,
               QString::fromStdString("Referent body: " + (ref_body==-1? "Center": bodies[ref_body]->ime)),
               QFont("Arial", 12, QFont::Bold));

    renderText(10, 60,
               QString::fromStdString("View body: " + (view_body==-1? "Center": bodies[view_body]->ime)),
               QFont("Arial", 12, QFont::Bold));

    if (view_mode == view_modes::SPHERE)
    {
        char rad_str[5];
        snprintf(rad_str, 5, "%.2f", view_radius);
        renderText(10, 80,
                   QString::fromStdString("View radius: " + string(rad_str)),
                   QFont("Arial", 12, QFont::Bold));
    }

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

int GLWidget::inc(int number, int max, int skip_number)
{
    if (number+1 < max)
    {
        if (number+1 != skip_number)
            return number+1;
        else if (number+2 < max)
            return number+2;
    }
    return number;
}

int GLWidget::dec(int number, int min, int skip_number)
{
    if (number-1 >= min)
    {
        if (number-1 != skip_number)
            return number-1;
        else if (number-2 >= min)
            return number-2;
    }
    return number;
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
        ref_body = inc(ref_body, bodies.size(), view_body);
        break;

    case Qt::Key_Down:
        ref_body = dec(ref_body, -1, view_body);
        break;

    case Qt::Key_Left:
        view_body = dec(view_body, -1, ref_body);
        break;

    case Qt::Key_Right:
        view_body = inc(view_body, bodies.size(), ref_body);
        break;

    case Qt::Key_Z:
        if (view_radius >= 0.1)
            view_radius -= 0.05;
        break;

    case Qt::Key_X:
        if (view_radius <= 0.9)
            view_radius += 0.05;
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
