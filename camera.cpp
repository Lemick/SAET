#include "camera.h"

Camera::Camera(bool& isSuccessfully)
{
    isSuccessfully = true;
    std::string videoStreamAddress;

    videoStreamAddress = loadCamAddress().toStdString();
    vcap = new VideoCapture();

    // Ouvre le flux vidéo et verifie qu'il est bien ouvert
    if(!vcap->open(videoStreamAddress)) {
        std::cout << "ERROR : Erreur dans l'ouverture du flux, verifiez l'adresse IP ou le peripherique de capture video" << std::endl;
        isSuccessfully = false;
    }
}

/// Utilise la librairie C++ d'OpenCV pour lire un flux vidéo
int Camera::readCamera(Mat& frame)
{
    //Lit l'image de la caméra
    if(!vcap->read(frame)) {
        cout << "Pas de frame recupere(ReadCamera)" << endl;
        return -1;
    }
    return 0;
}


void Camera::resizeFrame(Mat &frame, double ratio)
{
    resize(frame, frame, Size(vcap->get(CV_CAP_PROP_FRAME_WIDTH)*ratio
                            , vcap->get(CV_CAP_PROP_FRAME_HEIGHT)*ratio), 0, 0, INTER_CUBIC);
}

//Recupére l'adresse de la caméra dans le fichier config
QString Camera::loadCamAddress()
{
    QSettings appSettings(QString("./data/config/config.ini"), QSettings::IniFormat);
    QString addressCam = appSettings.value("CAM/address","r").toString();

    return addressCam;
}
