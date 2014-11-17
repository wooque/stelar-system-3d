#include "Tekstura.h"
#include <QGLWidget>
#include <QColor>

Tekstura::~Tekstura()
{
    if( id_teksture )
        glDeleteTextures(1, &id_teksture);
}

void Tekstura::primeniTeksturu()
{
    if( ! id_teksture )
    {
        if( tip_teksture == GL_TEXTURE_1D)
            napraviTeksturu1D();
        else
            napraviTeksturu2D();
    }

    glBindTexture(tip_teksture, id_teksture);
    glEnable(tip_teksture);
}

void Tekstura::ponistiTeksturu()
{
    if( id_teksture )
        glDisable(tip_teksture);
}

QImage Tekstura::citajFajl()
{
    QImage t;
    QImage b;

    if ( !b.load( imeFajla ) )
    {
        b = QImage( 16, 16, QImage::Format_RGB32 );
        b.fill( QColor(Qt::black).rgb() );
    }

    return QGLWidget::convertToGLFormat( b );
}

void Tekstura::napraviTeksturu1D()
{
    glGenTextures(1, &id_teksture);

    auto podaciTeksture = citajFajl();

    glBindTexture(GL_TEXTURE_1D, id_teksture);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, podaciTeksture.width(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, podaciTeksture.bits());

    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void Tekstura::napraviTeksturu2D()
{
    glGenTextures(1, &id_teksture);

    auto podaciTeksture = citajFajl();

    glBindTexture(GL_TEXTURE_2D, id_teksture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, podaciTeksture.width(), podaciTeksture.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, podaciTeksture.bits());

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
}
