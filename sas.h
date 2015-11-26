#ifndef SAS_H
#define SAS_H

#include <QObject>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv2/nonfree/features2d.hpp"

#include "panneaucirculaire.h"
#include "vehicule.h"
#include "mainwindow.h"

using namespace cv;
using std::cout;
using std::endl;

#define NB_CLASS_CIRC 3
#define NB_CLASS_AGGLO 2

class PanneauCirculaire;
class PanneauAgglomeration;

class SAS : public QObject
{
    Q_OBJECT

public:
    SAS(Vehicule* &leVehicule);

    int detectPanneaux(Mat &frame);
    void  isolatePanneauxCirculaires(Mat src, vector<PanneauCirculaire*>& vecPanneau_c);
    void  isolatePanneauxAgglomeration(Mat src, vector<PanneauAgglomeration*>& vecPanneau_a);
    void predictClassCirculaire(vector<PanneauCirculaire *> &myVecPanneau);
    void predictClassAgglomeration(vector<PanneauAgglomeration *>& myVecPanneau);
    void majPanneauxCirculaires(vector<PanneauCirculaire *> &vecPanneau_c);
    void majPanneauxAgglomeration(vector<PanneauAgglomeration *>& vecPanneau_a );
    void putTextOnDetection(Mat& frame, const vector<PanneauCirculaire *> vecPanneau_c, const vector<PanneauAgglomeration *> vecPanneau_a );
    void putRectOnDetection(Mat &frame, const vector<Rect> rect_c, const vector<Rect> rect_a);
    void majSleepingDetectionCirculaire(vector<PanneauCirculaire*>& vecPanneau_c);
    void majSleepingDetectionAgglomeration(vector<PanneauAgglomeration *> &vecPanneau_a);
    int majVitesseLimite(vector<PanneauCirculaire*>& vecPanneau_c,vector<PanneauAgglomeration*>& vecPanneau_a );
    bool readVocabulary(const string &filename, Mat &vocabulary);

    vector<PanneauCirculaire*> vecPanneau_c; //Contient tout les panneaux circulaires detéctés
    vector<PanneauAgglomeration*> vecPanneau_a; // Contient les panneaux agglos detectés
    vector<Rect> signs_c; // coord Panneaux circulaires
    vector<Rect> signs_a; // coord Panneaux agglomeration


private:
    String circulaire_cascade_name;
    String circulaire_svm_name;
    String circulaire_vocabulary_name;
    String agglomeration_cascade_name;
    CascadeClassifier cascade_panneau_circ;
    CascadeClassifier cascade_panneau_agglo;
    Ptr<DescriptorMatcher> matcher_c;
    Ptr<DescriptorExtractor> extractor_c;
    SurfFeatureDetector detector;
    BOWImgDescriptorExtractor* bowCirculaire; //Descriptor
    vector<KeyPoint> keypoints;
    CvSVM svm_circulaire;
    Mat vocabulary_c;

    Vehicule* monVehicule;
signals:
    void sigLimiteAutorise(int laLimiteAutorise);
};

#endif // SAS_H
