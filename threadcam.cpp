#include "threadcam.h"
#include "panneauagglomeration.h"

#include "Timer.h" // suppr



ThreadCam::~ThreadCam()
{
    delete camera;
}

ThreadCam::ThreadCam(Vehicule* &leVehicule)
{
    monVehicule = leVehicule;

    //Initialisation timer de gestion du nombre d'images par seconde
    timerFps = new QTimer();
    connect(timerFps,SIGNAL(timeout()),this,SLOT(sltTimerFpsTick()),Qt::DirectConnection);
    timerFps->start(1000);
    framePerSecond = 0;
}


void ThreadCam::run()
{
    bool isCamOpened = false;

    //camera = new Camera("http://192.168.2.187:8080/videofeed?dummy=param.mjpg", isCamOpened);
    //camera = new Camera("http://192.168.2.4/mjpg/video.mjpg",isCamOpened);
    camera = new Camera(isCamOpened);

    if (!isCamOpened) // Si le flux n'a pas été ouvert, arrêter le thread de gestion d'image
    {
        emit sigCamNotFound();
        this->terminate();
    }
    SAS* monSAS = new SAS(monVehicule);

    Mat frame, frameToShow;
    QPixmap pixFrame;
    int detect;
    int frameMaj = 0;

    while (1)
    {
        camera->readCamera(frame); // ~5ms
        camera->resizeFrame(frame,0.55); //~10ms
        frame.copyTo(frameToShow);

        detect = monSAS->detectPanneaux(frame); //~100ms

        if (monSAS->vecPanneau_c.size() >0)
            monSAS->majSleepingDetectionCirculaire(monSAS->vecPanneau_c); //~10ms
        if (monSAS->vecPanneau_a.size() >0)
            monSAS->majSleepingDetectionAgglomeration(monSAS->vecPanneau_a);//~10ms


        if( detect == 0)
        {
            monSAS->putRectOnDetection(frameToShow,monSAS->signs_c,monSAS->signs_a);
            monSAS->putTextOnDetection(frameToShow,monSAS->vecPanneau_c,monSAS->vecPanneau_a);

            if (monSAS->vecPanneau_c.size() > 0)
            {
                monSAS->isolatePanneauxCirculaires(frame,monSAS->vecPanneau_c);
                monSAS->predictClassCirculaire(monSAS->vecPanneau_c);
                monSAS->majPanneauxCirculaires(monSAS->vecPanneau_c);
            }
            if (monSAS->vecPanneau_a.size() > 0)
            {
                monSAS->isolatePanneauxAgglomeration(frame,monSAS->vecPanneau_a);
                monSAS->predictClassAgglomeration(monSAS->vecPanneau_a);
                monSAS->majPanneauxAgglomeration(monSAS->vecPanneau_a);
            }
        }

        if (frameMaj == 10) // Toutes les 10 frames Maj de la vitesse max
        {
            monSAS->majVitesseLimite(monSAS->vecPanneau_c,monSAS->vecPanneau_a);
            frameMaj = 0;
        }
        frameMaj++;
        framePerSecond++;
        pixFrame = ASM::cvMatToQPixmap(frameToShow);
        emit sigSendCamImg(pixFrame);
    }
}

void ThreadCam::sltTimerFpsTick()
{
    emit sigSendCamFps(framePerSecond);
    framePerSecond = 0;
}


