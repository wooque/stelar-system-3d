#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "GL/gl.h"
#include "glaux.h"

#include "NebeskoTelo.h"
#include "Tekstura.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
using std::unique_ptr;
using std::move;

unique_ptr<Zvezda> Sunce;
unique_ptr<Zvezda> pozadina;

long  prethodno_vreme = 0;

GLfloat AmbijentalnoSvetlo[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat DifuznoSvetlo[] =      { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat PozicijaSvetla[] =     { 0.0f, 0.0f, 0.0f, 1.0f };

unsigned pogled = 0;
int visina = 40;
int planeta = -1;
int prethodno_y = -1;

void reshape(int width, int height)
{
  glViewport(0, 0, (GLint) width, (GLint) height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width/(GLfloat)height, 1, 1200);
  
  glMatrixMode(GL_MODELVIEW);
}

//void mouse_motion(int x, int y)
//{
//	if(prethodno_y == -1)
//	{
//		prethodno_y = y;
//		return;
//	}
	
//	int temp = y - prethodno_y;
	
//	if(temp > 0)
//		visina++;
//	if(temp < 0)
//		visina--;
	
//	prethodno_y = y;
//}

//void mouse_action(int button, int state, int x, int y)
//{
//	switch(button)
//	{
//		case GLUT_LEFT_BUTTON:
//			switch(state)
//			{
//				case GLUT_DOWN: break;
//				case GLUT_UP: break;
//			}
//			break;

//		case GLUT_RIGHT_BUTTON:
//			switch(state)
//			{
//				case GLUT_DOWN: break;
//				case GLUT_UP: break;
//			}
//			break;
//	}
//}

//void special_key_down(int key, int x, int y)
//{
//	switch(key)
//	{
//		case GLUT_KEY_UP:
//			if(planeta < Sunce->broj_satelita() - 1)
//				planeta++;
//			break;

//		case GLUT_KEY_DOWN:
//			if(planeta >= 0)
//				planeta --;
//			break;

//		case GLUT_KEY_LEFT:
//			if (pogled > 0)
//				pogled--;
//			break;

//		case GLUT_KEY_RIGHT:
//			if (pogled < Sunce->broj_satelita())
//				pogled++;
//			break;

//    case GLUT_KEY_F1: break;
//    case GLUT_KEY_F2: break;
//    case GLUT_KEY_F3: break;
//    case GLUT_KEY_F4: break;
//	}
//}

//void key_down(unsigned char key, int x, int y)
//{
//	switch(key)
//	{
//		// ESC
//		case 27:
//		  exit(0);
//		  break;

//		// razmak
//		case 32: break;
//		case '1': break;
//	}
//}

void init_GL(void)
{
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

void init_sistem() {
  pozadina = unique_ptr<Zvezda>(new Zvezda(200, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/zvezde.bmp"))));
  Sunce = unique_ptr<Zvezda>(new Zvezda(12, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/gzvezda.bmp")) ));
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 0.3f, 0.05f, 14, 0.01, 0, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/merkur.bmp") )) ));
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 0.93f, 0.03f, 24, 0.02, 0, 30, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/venera.bmp") ))) );
  auto p = unique_ptr<Planeta>(new Planeta( 1, 0.01f, 35, 0.03,  23.5, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/zemlja.bmp"))));
  p->dodajSatelit( unique_ptr<Planeta>(new Planeta( 0.2f, 0.1f, 5, 0.04,  0, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/mesec.bmp"))) ));
  Sunce->dodajSatelit( move(p) );
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 0.88f, 0.007f, 43, 0.05,  0, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/mars.bmp") )) ));
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 6.5f, 0.002f, 60, 0.06,  60, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/jupiter.bmp")) ) ));
  //Sunce->dodajSatelit( new Planeta( 4.7f, 0.0009f, 100, 0.07,  12, 0, new BMPTekstura2D("teksture/saturn.bmp") ) );
  Sunce->dodajSatelit( unique_ptr<Planeta>(new PlanetaSaPrstenovima( 4.7f, 0.0009f, 100, 0.07,  0, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/saturn.bmp")), 1.5, 2.5, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/saturn-prsten.bmp")), 3) ));
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 3.3f, 0.0005f, 150, 0.08,  98, 0, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/uran.bmp") ))) );
  Sunce->dodajSatelit( unique_ptr<Planeta>(new Planeta( 2.7f, 0.0003f, 190, 0.09,  0, 10, unique_ptr<BMPTekstura2D>(new BMPTekstura2D("../teksture/neptun.bmp") ))) );
}

void draw(void)
{
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
  {
      proteklo = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - prethodno_vreme;

//      if( proteklo < 40 )
//      {
//          sleep_for(milliseconds( 40-proteklo ));
//          glFinish();
//          return;
//      }
  }
  else
      prethodno_vreme = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

  pozadina->crtaj();
  Sunce->crtaj();
  Sunce->pomeri(proteklo);

  prethodno_vreme += proteklo;

//  glFinish();
//  glutSwapBuffers();
}

//int main(int argc, char **argv)
//{
//  init_sistem();
////  glutInit(&argc, argv);
////  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
////  glutInitWindowSize(500, 500);
////  glutInitWindowPosition(100, 100);
////  glutCreateWindow(argv[0]);
//  init_GL();
//  glutDisplayFunc(draw);
//  glutIdleFunc(draw);
//  glutReshapeFunc(reshape);
////  glutKeyboardFunc(key_down);
////  glutSpecialFunc(special_key_down);
////  glutMouseFunc( mouse_action );
////  glutMotionFunc( mouse_motion );
////  glutPassiveMotionFunc( mouse_motion );
////  glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
////  glutFullScreen();
////  glutMainLoop();

//  return 0;
//}
