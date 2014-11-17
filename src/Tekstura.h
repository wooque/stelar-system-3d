#pragma once

#include "GL/gl.h"
#include <QString>
#include <QImage>

class Tekstura
{
private:
    GLuint  id_teksture;
    GLenum  tip_teksture;
    QString imeFajla;
    QImage citajFajl();

public:
    Tekstura(QString _imeFajla, GLenum tip = GL_TEXTURE_2D)
        : id_teksture(0), tip_teksture(tip), imeFajla(_imeFajla) {}

    ~Tekstura();

    void napraviTeksturu1D();
    void napraviTeksturu2D();
    void primeniTeksturu();
    void ponistiTeksturu();
};
