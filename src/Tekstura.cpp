#include "Tekstura.h"

#include <fstream>

using std::string;
using std::unique_ptr;
using std::ifstream;

Tekstura::~Tekstura()
{
      if( id_teksture )
            glDeleteTextures(1, &id_teksture);
}

void Tekstura::primeniTeksturu()
{
      if( ! id_teksture )
            napraviTeksturu();

      glBindTexture(tip_teksture, id_teksture);
      glEnable(tip_teksture);
}

void Tekstura::ponistiTeksturu()
{
      if( id_teksture )
            glDisable(tip_teksture);
}

unique_ptr<AUX_RGBImageRec> BMPTekstura::citajFajl()
{
      ifstream file;

      file.open(imeFajla);
      if( ! file.is_open() )
            return 0;

      return unique_ptr<AUX_RGBImageRec>(auxDIBImageLoad(imeFajla));
}

void BMPTekstura1D::napraviTeksturu()
{
      glGenTextures(1, &id_teksture);

      auto podaciTeksture = citajFajl();
      if( ! podaciTeksture )
            return;

      glBindTexture(GL_TEXTURE_1D, id_teksture);

      glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, podaciTeksture->sizeX, 0, 
                   GL_BGR, GL_UNSIGNED_BYTE, podaciTeksture->data);

      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void BMPTekstura2D::napraviTeksturu()
{
      glGenTextures(1, &id_teksture);

      auto podaciTeksture = citajFajl();
      if( ! podaciTeksture )
            return;

      glBindTexture(GL_TEXTURE_2D, id_teksture);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, podaciTeksture->sizeX, podaciTeksture->sizeY, 0, 
                   GL_BGR, GL_UNSIGNED_BYTE, podaciTeksture->data);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
}
