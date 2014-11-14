#pragma once

#include "Tekstura.h"

#include <deque>
#include <memory>
#include <vector>

class Planeta;

class NebeskoTelo
{
public:
      std::unique_ptr<Tekstura> tekstura;
      float       poluprecnik;
      float       ugao_revolucije;
      float       brzina_revolucije;
      float       poluprecnik_revolucije;
	  float		  brzina_rotacije;
	  float		  ugao_rotacije;
	  float       nagib;
	  float       nagib_ravni;
	  std::deque<float> segmenti;
	  GLfloat	  red;
	  GLfloat	  green;
	  GLfloat	  blue;
	  unsigned    kapacitet;
	  std::vector<std::unique_ptr<NebeskoTelo>> sateliti;

public:
      NebeskoTelo( float arg_poluprecnik, 
                   float arg_brzina_revolucije,
                   float arg_poluprecnik_revolucije,
				   float arg_brzina_rotacije,
				   float arg_nagib,
				   float arg_nagib_ravni,
				   int broj_segmenata,
				   std::unique_ptr<Tekstura> &&t)
      : tekstura(std::move(t)),
        poluprecnik(arg_poluprecnik),
        ugao_revolucije(0),
        brzina_revolucije(arg_brzina_revolucije), 
		poluprecnik_revolucije(arg_poluprecnik_revolucije),
		brzina_rotacije(arg_brzina_rotacije),
        ugao_rotacije(0),
		nagib(arg_nagib),
        nagib_ravni(arg_nagib_ravni),
		segmenti(),
		kapacitet(broj_segmenata) {}

      virtual ~NebeskoTelo() {}
      	  
      static void crtajSferu(float radius, int br_segmenata);
      virtual void crtajTelo() = 0;

      void postaviTeksturu(std::unique_ptr<Tekstura> &&t)
      {
            tekstura = std::move(t);
      }

      virtual void crtaj();
      void pomeri(int proteklo_vreme);
      
      void dodajSatelit( std::unique_ptr<NebeskoTelo> p );
	  NebeskoTelo &getSatelit( unsigned i );
	  int broj_satelita() { return sateliti.size(); }
};

class Zvezda : public NebeskoTelo
{	  
protected:
      virtual void crtajTelo();
      
public:
      Zvezda( float arg_poluprecnik, std::unique_ptr<Tekstura> &&t )
      : NebeskoTelo( arg_poluprecnik, 0, 0, 0, 0, 0, 0, std::move( t ) ) {}
      
      virtual ~Zvezda() {}
};

class Planeta : public NebeskoTelo
{
protected:
      virtual void crtajTelo();
      
public:
      Planeta(float arg_poluprecnik, 
              float arg_brzina_revolucije, 
              float arg_poluprecnik_revolucije, 
			  float arg_brzina_rotacije,
			  float arg_nagib,
			  float arg_nagib_ravni,
              std::unique_ptr<Tekstura> &&t)
			  : NebeskoTelo( arg_poluprecnik,
							 arg_brzina_revolucije,
							 arg_poluprecnik_revolucije,
							 arg_brzina_rotacije,
							 arg_nagib,
							 arg_nagib_ravni,
							 100,
							 std::move(t)) {}
							  
	  virtual ~Planeta() {}
};

class PlanetaSaPrstenovima: public Planeta
{
protected:
	float unut_r;
	float spolj_r;
	std::unique_ptr<Tekstura> tex_prsten;
	float prsten_n;
	
protected:
	virtual void crtajTelo();
	
public:
	  PlanetaSaPrstenovima(float arg_poluprecnik, 
              float arg_brzina_revolucije, 
              float arg_poluprecnik_revolucije, 
			  float arg_brzina_rotacije,
			  float arg_nagib,
			  float arg_nagib_ravni,
              std::unique_ptr<Tekstura> &&t, 
			  float arg_unut_r,
			  float arg_spolj_r,
			  std::unique_ptr<Tekstura> &&arg_tex_prsten,
			  float arg_prsten_n)
			  : Planeta( arg_poluprecnik,
						 arg_brzina_revolucije,
						 arg_poluprecnik_revolucije,
						 arg_brzina_rotacije,
						 arg_nagib,
						 arg_nagib_ravni,
						 std::move(t) ), 
			    unut_r(arg_unut_r),
			    spolj_r(arg_spolj_r),
			    tex_prsten(std::move(arg_tex_prsten)),
			    prsten_n(arg_prsten_n) {}
			  
	  virtual ~PlanetaSaPrstenovima() {}
};
