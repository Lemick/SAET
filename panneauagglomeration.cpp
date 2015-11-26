#include "panneauagglomeration.h"

PanneauAgglomeration::PanneauAgglomeration(Point leCentre, float laLargeur, float laHauteur)
{
    center = leCentre;
    largeur = laLargeur;
    hauteur = laHauteur;
    isChecked = false;
    nbVerif = 0;
    typePanneauAgglomeration = INCONNU;
}
