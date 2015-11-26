#ifndef PANNEAU_H
#define PANNEAU_H


#include <opencv2/core/core.hpp>

class SAS;

using cv::Point;
using cv::Mat;
using std::vector;

class Panneau
{

protected:
    Point center; // coordonnées du centre du panneau
    int nbVerif; // nb de fois qu'on a testé le panneau
    int nbNonDetect; //nb de frames ou l'on n'a pas detecté le panneau
    bool isChecked; //Le panneau est t'il verifié
    vector <float> vecClass; // Tableau contenant tout les predictions de classes
    Mat ROI; // region d'interet de l'image

};

#endif // PANNEAU_H
