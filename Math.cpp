#include "Math.h"
#include "iostream"

using std::cout;
using std::endl;

namespace Math
{
    int distance(const Point p1, const Point p2)
    {
        return(sqrtf((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)));
    }

    float calculAngleDegre(const Point2f p1, const Point2f p2)
    {
        float cDif=0;
        float angleDeg ;

        // Calcul du coeff directeur
        if (p2.x - p1.x  != 0)
            cDif = (p2.y - p1.y)/(p2.x - p1.x);
        else
            return 90.0;

        // Calcul de l'angle
        angleDeg = atanf(cDif) * 180.0f/3.1415f;

        //cout << "deg:" << angleDeg << endl;
        return angleDeg;
    }

}
