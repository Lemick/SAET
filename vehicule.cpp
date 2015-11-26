#include "vehicule.h"


Vehicule::Vehicule(Alarme*& lAlarme)
{
    monAlarme = new Alarme(this);
    compteur =0;
    limiteAutorise =0;
    isAccelere = true;
    lAlarme = monAlarme;
}

Vehicule::~Vehicule()
{
    delete monAlarme;
}


void Vehicule::set_compteur(int leCompteur)
{
    compteur = leCompteur;
}


int  Vehicule::get_compteur ()
{
    return compteur;
}

void Vehicule::set_limiteAutorise(int laLimiteAutorise)
{
    limiteAutorise = laLimiteAutorise;
}


int Vehicule::get_limiteAutorise()
{
    return limiteAutorise;
}


void Vehicule::sltReceptionLimiteAutorise(int laLimiteAutorise)
{
    limiteAutorise = laLimiteAutorise;
}

