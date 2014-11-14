#pragma once

#include "GL/gl.h"
#include "glaux.h"

#include <string>
#include <memory>

class Tekstura
{
protected:
      GLuint      id_teksture;
      GLenum      tip_teksture;
public:
      Tekstura(GLenum tip): id_teksture(0), tip_teksture(tip) {}
      virtual ~Tekstura();

      virtual void napraviTeksturu() = 0;
      virtual void primeniTeksturu();
	  virtual void ponistiTeksturu();
};

class BMPTekstura : public Tekstura
{
protected:
      std::string imeFajla;
      std::unique_ptr<AUX_RGBImageRec> citajFajl();

public:
      BMPTekstura(std::string _imeFajla, GLenum tip)
      : Tekstura(tip), imeFajla(_imeFajla) {}
};

class BMPTekstura1D : public BMPTekstura
{
public:
      BMPTekstura1D(std::string _imeFajla)
      : BMPTekstura(_imeFajla, GL_TEXTURE_1D) {}

      virtual void napraviTeksturu();
};

class BMPTekstura2D : public BMPTekstura
{
public:
      BMPTekstura2D(std::string _imeFajla)
      : BMPTekstura(_imeFajla, GL_TEXTURE_2D) {}

      virtual void napraviTeksturu();
};
