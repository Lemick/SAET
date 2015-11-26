#ifndef PANNEAUAGGLOMERATION_H
#define PANNEAUAGGLOMERATION_H

#include "panneau.h"

class PanneauAgglomeration : public Panneau
{
    friend class SAS;

public:
    PanneauAgglomeration(cv::Point leCentre,float laLargeur, float laHauteur);

    enum enumLimPanneauAgglomeration {INCONNU, ENTREE_AGGLO, SORTIE_AGGLO};
private:
    float largeur;
    float hauteur;
    int typePanneauAgglomeration;


};

#endif // PANNEAUAGGLOMERATION_H
