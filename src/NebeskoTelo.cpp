#include "NebeskoTelo.h"

#include <cmath>

//test
#include <iostream>

using std::unique_ptr;
using std::move;

void NebeskoTelo::crtajSferu(float radius, int br_segmenata)
{
    float step = 90.0f / br_segmenata;
    float x1, y1, z1, x2, y2, z2;
    float theta, phi;

    for( theta = 0; theta < 90; theta += step )
    {
        glBegin(GL_QUAD_STRIP);
        x1 = radius * cos(theta * RAD_PER_DEG);
        z1 = 0;
        y1 = radius * sin(theta * RAD_PER_DEG);

        x2 = radius * cos( (theta+step) * RAD_PER_DEG);
        z2 = 0;
        y2 = radius * sin( (theta+step) * RAD_PER_DEG);

        glNormal3f(x1, y1, z1);
        glTexCoord2f(0, 0.5f + theta/180);
        glVertex3f(x1, y1, z1);

        glNormal3f(x2, y2, z2);
        glTexCoord2f(0, 0.5f + (theta+step)/180);
        glVertex3f(x2, y2, z2);

        for( phi = step; phi <= 360; phi += step)
        {
            x1 = radius * cos(theta * RAD_PER_DEG) * cos(phi * RAD_PER_DEG);
            z1 = radius * cos(theta * RAD_PER_DEG) * sin(phi * RAD_PER_DEG);
            y1 = radius * sin(theta * RAD_PER_DEG);

            x2 = radius * cos((theta+step) * RAD_PER_DEG) * cos(phi * RAD_PER_DEG);
            z2 = radius * cos((theta+step) * RAD_PER_DEG) * sin(phi * RAD_PER_DEG);
            y2 = radius * sin((theta+step) * RAD_PER_DEG);

            glNormal3f(x1, y1, z1);
            glTexCoord2f(phi/360, 0.5f + theta/180);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2, y2, z2);
            glTexCoord2f(phi/360, 0.5f + (theta+step)/180);
            glVertex3f(x2, y2, z2);
        }

        glEnd();
    }


    for( theta = 0; theta > -90; theta -= step )
    {
        glBegin(GL_QUAD_STRIP);
        x1 = radius * cos(theta * RAD_PER_DEG);
        z1 = 0;
        y1 = radius * sin(theta * RAD_PER_DEG);

        x2 = radius * cos( (theta-step) * RAD_PER_DEG);
        z2 = 0;
        y2 = radius * sin( (theta-step) * RAD_PER_DEG);

        glNormal3f(x1, y1, z1);
        glTexCoord2f(0, 0.5f + theta/180);
        glVertex3f(x1, y1, z1);

        glNormal3f(x2, y2, z2);
        glTexCoord2f(0, 0.5f + (theta-step)/180);
        glVertex3f(x2, y2, z2);

        for( phi = step; phi <= 360; phi += step)
        {
            x1 = radius * cos(theta * RAD_PER_DEG) * cos(phi * RAD_PER_DEG);
            z1 = radius * cos(theta * RAD_PER_DEG) * sin(phi * RAD_PER_DEG);
            y1 = radius * sin(theta * RAD_PER_DEG);

            x2 = radius * cos((theta-step) * RAD_PER_DEG) * cos(phi * RAD_PER_DEG);
            z2 = radius * cos((theta-step) * RAD_PER_DEG) * sin(phi * RAD_PER_DEG);
            y2 = radius * sin((theta-step) * RAD_PER_DEG);

            glNormal3f(x1, y1, z1);
            glTexCoord2f(phi/360, 0.5f + theta/180);
            glVertex3f(x1, y1, z1);

            glNormal3f(x2, y2, z2);
            glTexCoord2f(phi/360, 0.5f + (theta-step)/180);
            glVertex3f(x2, y2, z2);

        }

        glEnd();
    }
}

void NebeskoTelo::crtaj() const
{
    glPushMatrix();
    glRotated( nagib_ravni, 1, 0, 0 );

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    for (unsigned i = 0; i < segmenti.size(); i++)
    {
        float clr = 0.75f * ((float)i)/segmenti.size();
        glColor4f(1, 1, 1, clr);
        float ugao = segmenti[i];
        glVertex3f(cos(RAD_PER_DEG * ugao) * poluprecnik_revolucije, 0, -sin(RAD_PER_DEG * ugao) * poluprecnik_revolucije);
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);

    if( tekstura )
        tekstura->primeniTeksturu();
    glRotated( ugao_revolucije, 0, 1, 0 );
    glTranslated( poluprecnik_revolucije, 0, 0 );
    glRotated( -ugao_revolucije, 0, 1, 0 );

    crtajTelo();

    if( tekstura )
        tekstura->ponistiTeksturu();

    for(const auto &satelit: sateliti)
        satelit->crtaj();

    glPopMatrix();
}

void NebeskoTelo::pomeri(int proteklo_vreme)
{
    ugao_revolucije += brzina_revolucije*proteklo_vreme;
    ugao_rotacije += brzina_rotacije*proteklo_vreme;

    for(const auto &satelit: sateliti)
        satelit->pomeri(proteklo_vreme);

    if (segmenti.size() > 0 && segmenti.size() == kapacitet)
        segmenti.pop_front();
    segmenti.push_back(ugao_revolucije);

}

std::pair<float, float> NebeskoTelo::getPos() const
{
    return std::make_pair<float, float>(
                cos(ugao_revolucije * RAD_PER_DEG) * poluprecnik_revolucije,
                -sin(ugao_revolucije * RAD_PER_DEG) * poluprecnik_revolucije);
}

void NebeskoTelo::crtajTelo() const
{
    if( !is_star )
        glEnable(GL_LIGHTING);
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
    }

    glPushMatrix();

    if( nagib > 0 )
        glRotated( nagib, 0, 0, -1 );

    glScaled( poluprecnik, poluprecnik, poluprecnik );

    if( ugao_rotacije > 0)
        glRotated(ugao_rotacije, 0, 1, 0);

    crtajSferu( 1, 9 );

    if (prsten)
        prsten->crtajPrsten();

    if (is_star)
        glEnable(GL_BLEND);

    glPopMatrix();
}

void NebeskoTelo::crtajSvetlo() const
{
    if(!svetlo)
        return;

    svetlo->crtajSvetlo(0.0f, 0.0f, 0.0f);
}

void Prsten::crtajPrsten() const
{
    glDisable(GL_LIGHTING);
//    glDisable(GL_DEPTH);
    glBegin(GL_QUAD_STRIP);
    if( tex_prsten )
        tex_prsten->primeniTeksturu();
    int seg = 90.0 / prsten_n;
    for ( unsigned i = 0; i <= 360; i+=seg )
    {
        float cos_t = cos(i * RAD_PER_DEG);
        float sin_t = sin(i * RAD_PER_DEG);
        glTexCoord2d(0, i);
        glVertex3d(cos_t * unut_r, 0, -sin_t * unut_r);
        glTexCoord2d(1, i);
        glVertex3d(cos_t * spolj_r, 0, -sin_t * spolj_r);
    }
    glEnd();
//    glEnable(GL_DEPTH);
}

void Svetlo::crtajSvetlo(float x, float y, float z) const
{
    GLfloat DifuznoSvetlo[] = { red, green, blue, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, DifuznoSvetlo);
    GLfloat PozicijaSvetla[] = { x, y, z, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, PozicijaSvetla);
}
