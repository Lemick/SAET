#ifndef VEHICULE_H
#define VEHICULE_H

#include <QTimer>
#include <QObject>

#include "alarme.h"
#include "sas.h"
#include "mainwindow.h"

class Alarme;

class Vehicule : public QObject
{
    Q_OBJECT

public:
    Vehicule(Alarme*& lAlarme);
    ~Vehicule();
    void set_compteur(int leCompteur);
    int  get_compteur ();
    void set_limiteAutorise(int laLimiteAutorise);
    int get_limiteAutorise();

private:
    int compteur;
    Alarme* monAlarme;
    bool isAccelere;
    int limiteAutorise;
private slots:
    void sltReceptionLimiteAutorise(int laLimiteAutorise);


};

#endif // VEHICULE_H
