#include "glwidget.h"

#include <QTimer>
#include <QApplication>
#include <cmath>
#include <chrono>

#include "GL/gl.h"
#include "GL/glu.h"

#include "Tekstura.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::unique_ptr;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    pozadina = unique_ptr<NebeskoTelo>(new NebeskoTelo(200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                                       unique_ptr<Tekstura>(new Tekstura("../teksture/zvezde.bmp")), true ));

    Sunce = unique_ptr<NebeskoTelo>(new NebeskoTelo(12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                                    unique_ptr<Tekstura>(new Tekstura("../teksture/gzvezda.bmp")), true ));

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 0.3f, 0.05f, 14.0f, 0.01f, 0.0f, 0.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/merkur.bmp") )) ));

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 0.93f, 0.03f, 24.0f, 0.02f, 0.0f, 30.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/venera.bmp") ))) );

    auto p = unique_ptr<NebeskoTelo>(new NebeskoTelo( 1.0f, 0.01f, 35.0f, 0.03f, 23.5f, 0.0f, 100,
                                                      unique_ptr<Tekstura>(new Tekstura("../teksture/zemlja.bmp"))));

    p->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 0.2f, 0.1f, 5.0f, 0.04f, 0.0f, 0.0f, 100,
                                                              unique_ptr<Tekstura>(new Tekstura("../teksture/mesec.bmp"))) ));
    Sunce->dodajSatelit( std::move(p) );
    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 0.88f, 0.007f, 43.0f, 0.05f, 0.0f, 0.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/mars.bmp") )) ));

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 6.5f, 0.002f, 60.0f, 0.06f, 60.0f, 0.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/jupiter.bmp")) ) ));

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 4.7f, 0.0009f, 100.0f, 0.07f, 0.0f, 0.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/saturn.bmp")),
                                                                  false,
                                                                  unique_ptr<Prsten>(new Prsten(1.5f, 2.5f, unique_ptr<Tekstura>(new Tekstura("../teksture/saturn-prsten.bmp")), 3))) ));

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 3.3f, 0.0005f, 150.0f, 0.08f, 98.0f, 0.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/uran.bmp") ))) );

    Sunce->dodajSatelit( unique_ptr<NebeskoTelo>(new NebeskoTelo( 2.7f, 0.0003f, 190.0f, 0.09f, 0.0f, 10.0f, 100,
                                                                  unique_ptr<Tekstura>(new Tekstura("../teksture/neptun.bmp") ))) );

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

    int view_x;
    int view_z;
    if(pogled == 0)
    {
        view_x = 0;
        view_z = 0;
    }
    else
    {
        auto &temp = Sunce->getSatelit(pogled - 1);
        view_x = cos(temp.ugao_revolucije * 3.1415 / 180) * temp.poluprecnik_revolucije;
        view_z = -sin(temp.ugao_revolucije * 3.1415 / 180) * temp.poluprecnik_revolucije;
    }

    int pos_x;
    int pos_z;
    if(planeta != -1)
    {
        auto &temp = Sunce->getSatelit(planeta);
        pos_x = cos(temp.ugao_revolucije * 3.1415 / 180) * temp.poluprecnik_revolucije;
        pos_z = -sin(temp.ugao_revolucije * 3.1415 / 180) * temp.poluprecnik_revolucije;
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
