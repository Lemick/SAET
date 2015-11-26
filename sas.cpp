#include "sas.h"
#include "panneauagglomeration.h"
#include "Math.h"



SAS::SAS(Vehicule* &leVehicule)
{
    circulaire_cascade_name ="./data/trained_files/cascade9.xml";
    circulaire_svm_name ="./data/trained_files/Bow_svm_alpha";
    circulaire_vocabulary_name = "./data/trained_files/vocabulary";
    agglomeration_cascade_name = "./data/trained_files/cascadeagglo3.xml";


    matcher_c= new FlannBasedMatcher;
    extractor_c = new SurfDescriptorExtractor();
    detector.hessianThreshold = 1000;
    bowCirculaire = new BOWImgDescriptorExtractor(extractor_c,matcher_c);

    readVocabulary(circulaire_vocabulary_name,vocabulary_c);
    bowCirculaire->setVocabulary(vocabulary_c);

    //-- Load the cascades
    if( !cascade_panneau_circ.load( circulaire_cascade_name ) ){ printf("ERROR: %s  non charge, verifiez que le fichier est bien a la racine du repertoire de compilation.\n",circulaire_cascade_name.c_str());};
    if( !cascade_panneau_agglo.load( agglomeration_cascade_name ) ){ printf("ERROR: %s  non charge, verifiez que le fichier est bien a la racine du repertoire de compilation.\n",agglomeration_cascade_name.c_str());};

    //--Load the SVM for limitations sign traffic and agglo
    svm_circulaire.load(circulaire_svm_name.c_str());

    this->monVehicule = leVehicule;
    connect(this,SIGNAL(sigLimiteAutorise(int)),monVehicule,SLOT(sltReceptionLimiteAutorise(int)));

}


int SAS::detectPanneaux( Mat& frame )
{
    Mat frame_gray;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect signs
    cascade_panneau_agglo.detectMultiScale( frame_gray, signs_a, 1.12, 5, 0|CV_HAAR_SCALE_IMAGE, Size(40, 10) );
    cascade_panneau_circ.detectMultiScale( frame_gray, signs_c, 1.15, 3,0|CV_HAAR_SCALE_IMAGE, Size(40, 40) );

    if(signs_c.size()== 0  &&  signs_a.size() == 0)
        return -1;


    for( size_t i = 0; i < signs_c.size(); i++ )
    {
        Point center( signs_c[i].x + signs_c[i].width*0.5,signs_c[i].y + signs_c[i].height*0.5 );
        float radius = (signs_c[i].width*0.4 + signs_c[i].height*0.4)/2;

        /// Dessin du contour du cercle
        //circle( frame, center, radius , Scalar(0,0,255), 3, 8, 0 );

        ///Si aucun panneau dans la liste a un centre a X pixel du centre detecté
        bool isNewSign = true;
        for(size_t it=0; it<vecPanneau_c.size(); ++it)
        {
            if (Math::distance(vecPanneau_c[it]->center, center) < 40)
            {
                vecPanneau_c[it]->center = center; // new center
                vecPanneau_c[it]->radius = radius;
                vecPanneau_c[it]->isChecked = true;   // a été trouvé sur l'image
                vecPanneau_c[it]->nbNonDetect = 0; // Remise a zéro du compteur puisque panneau detecté
                isNewSign = false;
                break;
            }
        }
        /// alors ajout du panneau dans les panneaux courants
        if(isNewSign)
        {
            vecPanneau_c.push_back(new PanneauCirculaire(center,radius));
            vecPanneau_c.back()->isChecked = true;
        }
    }

    for( size_t i = 0; i < signs_a.size(); i++ )
    {
        Point center( signs_a[i].x + signs_a[i].width*0.5,signs_a[i].y + signs_a[i].height*0.5 );
        float width = signs_a[i].width ;
        float height =signs_a[i].height;

        //rectangle(frame,signs_a[i],Scalar(255,0,0));

        ///Si aucun panneau dans la liste a un centre a 40 pixel du centre detecté
        bool isNewSign = true;
        for(size_t it=0; it<vecPanneau_a.size(); ++it)
        {
            if (Math::distance(vecPanneau_a[it]->center, center) < 100)
            {
                vecPanneau_a[it]->center = center; // new center
                vecPanneau_a[it]->hauteur = height;
                vecPanneau_a[it]->largeur = width;
                vecPanneau_a[it]->isChecked = true;   // a été trouvé sur l'image
                vecPanneau_a[it]->nbNonDetect = 0; // Remise a zéro du compteur puisque panneau detecté
                isNewSign = false;
                break;
            }
        }
        /// alors ajout du panneau dans les panneaux courants
        if(isNewSign)
        {
            vecPanneau_a.push_back(new PanneauAgglomeration(center ,width ,height));
            vecPanneau_a.back()->isChecked = true;
        }
    }
    return 0;
}


void SAS::isolatePanneauxCirculaires(const Mat src, vector<PanneauCirculaire *> &vecPanneau_c)
{

    Mat temp;
    Point center;
    int radius;
    Mat mask(src.size(),src.type());  // création d'un Mat aux mêmes dimensions que src

    // Add all found circles to mask and
    for( size_t i = 0; i < vecPanneau_c.size(); i++ )
    {
        mask.setTo(Scalar(0,0,0));
        center =  vecPanneau_c[i]->center;
        radius =  cvRound(vecPanneau_c[i]->radius);
        circle( mask, center , radius, Scalar(255,255,255),-1, 8, 0 );    // Circle(img, center, radius, color, thickness=1, lineType=8, shift=0)
        src.copyTo(temp,mask); // copie le masque to l'image masque (1 & 0 )
        vecPanneau_c[i]->ROI = src(Rect(center.x-radius,center.y-radius, radius*2, radius*2 )); // image redimensionné

    }
}

void SAS::isolatePanneauxAgglomeration(const Mat src, vector<PanneauAgglomeration*>& vecPanneau_a)
{

    for (size_t i = 0; i <vecPanneau_a.size(); i++)
    {
        vecPanneau_a[i]->ROI = src(Rect(Point(vecPanneau_a[i]->center.x - 0.5*vecPanneau_a[i]->largeur ,vecPanneau_a[i]->center.y - 0.5*vecPanneau_a[i]->hauteur)
                                              ,Point(vecPanneau_a[i]->center.x + 0.5*+vecPanneau_a[i]->largeur, vecPanneau_a[i]->center.y + 0.5*vecPanneau_a[i]->hauteur))); // image redimensionné
    }
}

void SAS::predictClassCirculaire(vector<PanneauCirculaire *>& myVecPanneau)
{
    Mat bowDescriptor;
    float response;

    for(size_t i=0; i<myVecPanneau.size(); ++i)
    {
        if (myVecPanneau[i]->typePanneauCirculaire == PanneauCirculaire::INCONNU) // Prediction de classe pour un panneau non classifié
        {
            detector.detect(myVecPanneau[i]->ROI, keypoints);
            bowCirculaire->compute(myVecPanneau[i]->ROI, keypoints, bowDescriptor);
            if (keypoints.size() > 1)
            {
                response = svm_circulaire.predict(bowDescriptor);
                cout << "response :  " << response << endl;

                myVecPanneau[i]->vecClass.push_back(response);
                myVecPanneau[i]->nbVerif++;
            }
        }
    }
}


void SAS::predictClassAgglomeration(vector<PanneauAgglomeration *>& myVecPanneau)
{
    Mat src,dst, cdst;

    for(size_t num=0; num<myVecPanneau.size(); ++num)
    {
        if (myVecPanneau[num]->typePanneauAgglomeration == PanneauAgglomeration::INCONNU) // Prediction de classe pour un panneau non classifié
        {
            myVecPanneau[num]->ROI.copyTo(src);
            Canny(src, dst, 50, 200, 3); // Applique une detection de contours
            cvtColor(dst, cdst, CV_GRAY2BGR); // Conversion en nuances de gris

            vector<Vec4i> lines, linesA;
            float degreLine;

            HoughLinesP(dst, lines, 1, CV_PI/180, 75, 50, 10 );
            for( size_t i = 0; i < lines.size(); i++ )
            {
                Vec4i l = lines[i];
                degreLine = Math::calculAngleDegre(Point2f(l[0], l[1]), Point2f(l[2], l[3])) ;
                if (degreLine < -9.0 && degreLine > -30.0) // Angle
                {
                    linesA.push_back(l);
                }
                else
                    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
            }
            float distanceTotale = 0;
            for(size_t i=0; i< linesA.size(); ++i)
            {
                distanceTotale += Math::distance(Point(linesA[i][0], linesA[i][1]), Point(linesA[i][2], linesA[i][3]));
            }
            if (distanceTotale >= src.cols*0.3) {
                cout << "Panneau non AGGLO    " << distanceTotale << endl;
                myVecPanneau[num]->vecClass.push_back((float)PanneauAgglomeration::SORTIE_AGGLO);
                myVecPanneau[num]->nbVerif++;
            }
            else {
                cout << "Panneau AGGLO" << endl;
                myVecPanneau[num]->vecClass.push_back((float)PanneauAgglomeration::ENTREE_AGGLO);
                myVecPanneau[num]->nbVerif++;
            }
        }
    }
}

//Met les informations relatives a la detection des panneaux à jour
void SAS::majPanneauxCirculaires(vector<PanneauCirculaire *>& vecPanneau_c )
{
    for(size_t i=0; i< vecPanneau_c.size(); ++i)
    {
        vecPanneau_c[i]->isChecked = false; // pour être ré évalué au prochain detect
        if( vecPanneau_c[i]->nbVerif == 4   &&  vecPanneau_c[i]->typePanneauCirculaire == PanneauCirculaire::INCONNU ) // si le panneau n'est pas encore evalué et qu'il a assez de donnés pour l'être
        {
            int tickClass[NB_CLASS_CIRC] = {0};
            for(int j=0; j<3; j++) // Parcourt les 4 prédictions de classes
            {
                if( ++tickClass[ ((int)vecPanneau_c[i]->vecClass[j]) - 1  ] == 3) //75%
                {
                    vecPanneau_c[i]->typePanneauCirculaire = (int)vecPanneau_c[i]->vecClass[j]; //un panneau est declaré comme étant un PANNEAU de classe
                    //std::cout << "un panneau est declaré comme étant un PANNEAU de classe " << vecPanneau_c[i]->limPanneau << endl;
                    break;
                }
            }
            //Si pas de resultat concluant arrêter la detection de panneau (supprimer)
            if (vecPanneau_c[i]->typePanneauCirculaire == PanneauCirculaire::INCONNU)
                vecPanneau_c.erase(vecPanneau_c.begin() +i);
        }
    }
}

void SAS::majPanneauxAgglomeration(vector<PanneauAgglomeration *>& vecPanneau_a)
{
    for(size_t i=0; i< vecPanneau_a.size(); ++i)
    {
        vecPanneau_a[i]->isChecked = false; // pour être ré évalué au prochain detect
        if( vecPanneau_a[i]->nbVerif == 5   &&  vecPanneau_a[i]->typePanneauAgglomeration == PanneauAgglomeration::INCONNU ) // si le panneau n'est pas encore evalué et qu'il a assez de donnés pour l'être
        {
            int tickClass[NB_CLASS_AGGLO] = {0};
            for(int j=0; j<4; j++) // Parcourt les X prédictions de classes
            {
                if( ++tickClass[ ((int)vecPanneau_a[i]->vecClass[j]) - 1  ] == 4) //80%
                {
                    vecPanneau_a[i]->typePanneauAgglomeration = (int)vecPanneau_a[i]->vecClass[j]; //un panneau est declaré comme étant un PANNEAU de classe
                    break;
                }
            }
            //Si pas de resultat concluant arrêter la detection de panneau (supprimer)
            if (vecPanneau_a[i]->typePanneauAgglomeration == PanneauAgglomeration::INCONNU)
                vecPanneau_a.erase(vecPanneau_a.begin() +i);
        }
    }
}

void SAS::majSleepingDetectionCirculaire(vector<PanneauCirculaire *> &vecPanneau_c)
{
    for(size_t i=0 ; i<vecPanneau_c.size();++i)
    {
        if ( (!vecPanneau_c[i]->isChecked) && vecPanneau_c[i]->nbNonDetect == 8) //X frames de marge pour redetecter le panneau
            vecPanneau_c.erase(vecPanneau_c.begin()+i); //delete the element
        else if (!vecPanneau_c[i]->isChecked)
            vecPanneau_c[i]->nbNonDetect++;
    }
}

void SAS::majSleepingDetectionAgglomeration(vector<PanneauAgglomeration *> &vecPanneau_a)
{
    for(size_t i=0 ; i<vecPanneau_a.size();++i)
    {
        if ( (!vecPanneau_a[i]->isChecked) && vecPanneau_a[i]->nbNonDetect == 1) //X frames de marge pour redetecter le panneau
            vecPanneau_a.erase(vecPanneau_a.begin()+i); //delete the element
        else if (!vecPanneau_a[i]->isChecked)
            vecPanneau_a[i]->nbNonDetect++;
    }
}

int SAS::majVitesseLimite(vector<PanneauCirculaire*>& vecPanneau_c,vector<PanneauAgglomeration*>& vecPanneau_a )
{
    if (vecPanneau_c.size() == 0 && vecPanneau_a.size() == 0)
        return 0;

    PanneauCirculaire panneauChoisi_c = PanneauCirculaire(Point(0,0),0);
    for(size_t i=0; i< vecPanneau_c.size(); i++)
    {
        if(vecPanneau_c[i]->radius > panneauChoisi_c.radius)
        {
            panneauChoisi_c = *vecPanneau_c[i];
        }
    }

    PanneauAgglomeration panneauChoisi_a = PanneauAgglomeration(Point(0,0),0,0);
    for(size_t i=0; i< vecPanneau_a.size(); i++)
    {
        if(vecPanneau_a[i]->largeur > panneauChoisi_a.largeur)
        {
            panneauChoisi_a = *vecPanneau_a[i];
        }
    }
    //ratio panneau_c vs panneau_a = x3.15
    if (panneauChoisi_c.radius*2*3.15 > panneauChoisi_a.largeur)
    {
        switch (panneauChoisi_c.typePanneauCirculaire)
        {
        case PanneauCirculaire::PANNEAU_30:
            emit sigLimiteAutorise(30);
            break;
        case PanneauCirculaire::PANNEAU_50:
            emit sigLimiteAutorise(50);
            break;
        case PanneauCirculaire::PANNEAU_70:
            emit sigLimiteAutorise(70);
            break;
        default:
            break;
        }
        return 1;
    }
    else
    {
        switch (panneauChoisi_a.typePanneauAgglomeration)
        {
        case PanneauAgglomeration::ENTREE_AGGLO:
            emit sigLimiteAutorise(50);
            break;
        case PanneauAgglomeration::SORTIE_AGGLO:
            emit sigLimiteAutorise(90);
            break;
        default:
            break;
        }
        return 1;
    }
}

//Entoure d'un rectangle les détéctions sur l'image affichée
void SAS::putRectOnDetection(Mat& frame,const vector<Rect> rect_c, const vector<Rect> rect_a )
{
    for(size_t i =0; i< rect_c.size(); ++i)
    {
        rectangle(frame,rect_c[i],Scalar(255,0,0));
    }
    for(size_t i =0; i< rect_a.size(); ++i)
    {
        rectangle(frame,rect_a[i],Scalar(125,0,0));
    }
}

//Place un texte sur la frame selon leur état de detection
void SAS::putTextOnDetection(Mat& frame, const vector<PanneauCirculaire *> vecPanneau_c, const vector<PanneauAgglomeration *> vecPanneau_a )
{
    double scaleTexte = 0.60;
    Scalar color = Scalar(0,150,177);
    for(size_t i =0; i< vecPanneau_c.size(); ++i)
    {
        Point ptText(vecPanneau_c[i]->center.x  - (0.7*vecPanneau_c[i]->radius)  //X
                     ,vecPanneau_c[i]->center.y - (0.7*vecPanneau_c[i]->radius)); //Y

        if (vecPanneau_c[i]->nbNonDetect == 0)
        {
            switch (vecPanneau_c[i]->typePanneauCirculaire)
            {
            case PanneauCirculaire::INCONNU:
                putText(frame,"lim ???",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            case PanneauCirculaire::PANNEAU_30:
                putText(frame,"lim 30",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            case PanneauCirculaire::PANNEAU_50:
                putText(frame,"lim 50",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            case PanneauCirculaire::PANNEAU_70:
                putText(frame,"lim 70",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            default:
                break;
            }
        }
    }
    for(size_t i =0; i< vecPanneau_a.size(); ++i)
    {
        Point ptText(vecPanneau_a[i]->center.x                                     //X
                     ,vecPanneau_a[i]->center.y - (0.5*vecPanneau_a[i]->hauteur)); //Y

        if (vecPanneau_a[i]->nbNonDetect == 0)
        {
            switch (vecPanneau_a[i]->typePanneauAgglomeration)
            {
            case PanneauAgglomeration::INCONNU:
                putText(frame,"Agglo ???",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            case PanneauAgglomeration::ENTREE_AGGLO:
                putText(frame,"Entree",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            case PanneauAgglomeration::SORTIE_AGGLO:
                putText(frame,"Sortie",ptText,FONT_HERSHEY_SIMPLEX,scaleTexte,color,2 );
                break;
            default:
                break;
            }
        }
    }
}

bool SAS::readVocabulary( const string& filename, Mat& vocabulary )
{
    cout << "Reading vocabulary..." << endl;
    FileStorage fs( filename, FileStorage::READ );
    if( fs.isOpened() )
    {
        fs["vocabulary"] >> vocabulary;
        cout << "done" << endl;
        return true;
    }
    cout <<"ERROR: " << filename << " non trouve a la racine du repertoire de compilation "<<endl;
    return false;
}
