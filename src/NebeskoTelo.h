#pragma once

#include "Tekstura.h"
#include "glwidget.h"
#include <deque>
#include <memory>
#include <vector>
#include <utility>
#include <cmath>

const float RAD_PER_DEG = M_PI/180;

class Prsten
{
private:
    float                       unut_r;
    float                       spolj_r;
    std::unique_ptr<Tekstura>   tex_prsten;
    float                       prsten_n;

public:
    Prsten(float arg_unut_r,
           float arg_spolj_r,
           std::unique_ptr<Tekstura> arg_tex_prsten,
           float arg_prsten_n)
        : unut_r(arg_unut_r),
          spolj_r(arg_spolj_r),
          tex_prsten(std::move(arg_tex_prsten)),
          prsten_n(arg_prsten_n) {}

    ~Prsten() {}

    void crtajPrsten() const;
};

class Svetlo
{
public:
    float   red;
    float   green;
    float   blue;

public:
    Svetlo(float arg_red,
           float arg_green,
           float arg_blue)
        : red(arg_red),
          green(arg_green),
          blue(arg_blue) {}

    ~Svetlo() {}

    void crtajSvetlo(float x, float y, float z) const;
};

class NebeskoTelo
{
public:
    std::string                                 ime;
private:
    std::unique_ptr<Tekstura>                   tekstura;
    float                                       poluprecnik;
    float                                       ugao_revolucije;
    float                                       brzina_revolucije;
    float                                       poluprecnik_revolucije;
    float                                       brzina_rotacije;
    float                                       ugao_rotacije;
    float                                       nagib;
    float                                       nagib_ravni;
    std::deque<float>                           segmenti;
    unsigned                                    kapacitet;
    std::vector<std::unique_ptr<NebeskoTelo>>   sateliti;
    bool                                        is_star;
    std::unique_ptr<Prsten>                     prsten;
    std::unique_ptr<Svetlo>                     svetlo;

public:
    NebeskoTelo( std::string arg_ime,
                 float arg_poluprecnik,
                 float arg_brzina_revolucije,
                 float arg_poluprecnik_revolucije,
                 float arg_brzina_rotacije,
                 float arg_nagib,
                 float arg_nagib_ravni,
                 int broj_segmenata,
                 std::unique_ptr<Tekstura> t,
                 bool arg_is_star = false,
                 std::unique_ptr<Prsten> p = nullptr,
                 std::unique_ptr<Svetlo> s = nullptr)
        : ime(arg_ime),
          tekstura(std::move(t)),
          poluprecnik(arg_poluprecnik),
          ugao_revolucije(0),
          brzina_revolucije(arg_brzina_revolucije),
          poluprecnik_revolucije(arg_poluprecnik_revolucije),
          brzina_rotacije(arg_brzina_rotacije),
          ugao_rotacije(0),
          nagib(arg_nagib),
          nagib_ravni(arg_nagib_ravni),
          segmenti(),
          kapacitet(broj_segmenata),
          is_star(arg_is_star),
          prsten(std::move(p)),
          svetlo(std::move(s))
    {}

    ~NebeskoTelo() {}

    static void crtajSferu(float radius, int br_segmenata);
    void crtaj(GLWidget *glw) const;
    void crtajTelo() const;

    void pomeri(int proteklo_vreme);

    void dodajSatelit( std::unique_ptr<NebeskoTelo> p )
    {
        sateliti.push_back( move(p) );
    }

    int broj_satelita() const
    {
        return sateliti.size();
    }

    void dodajPrsten( std::unique_ptr<Prsten> arg_prsten )
    {
        prsten = std::move(arg_prsten);
    }

    void crtajSvetlo() const;

    std::pair<float, float> getPos() const;
};
