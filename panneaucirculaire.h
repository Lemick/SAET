#ifndef PANNEAUCIRCULAIRE_H
#define PANNEAUCIRCULAIRE_H

#include "panneau.h"


using cv::Point;


class PanneauCirculaire : public Panneau
{
    friend class SAS;
public:
    PanneauCirculaire(Point leCentre, float leRayon);

    enum  Type {INCONNU, PANNEAU_30, PANNEAU_50, PANNEAU_70};
private:
    float radius; // rayon
    int typePanneauCirculaire;
};

#endif // PANNEAUCIRCULAIRE_H
