#include "panneaucirculaire.h"

PanneauCirculaire::PanneauCirculaire(Point leCentre, float leRayon)
{
    center = leCentre;
    radius = leRayon;
    isChecked = false;
    nbVerif = 0;
    nbNonDetect = 0;
    typePanneauCirculaire = INCONNU;
}
