#ifndef THREADCAM_H
#define THREADCAM_H

#include <QTimer>
#include <QThread>
#include <QObject>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "asmOpenCV.h"
#include "camera.h"
#include "sas.h"
#include "vehicule.h"


using namespace cv;
using std::cout;
using std::endl;

class Vehicule;

class ThreadCam : public QThread
{
    Q_OBJECT
public:
    ThreadCam(Vehicule *&leVehicule);
    ~ThreadCam();
    QTimer* timerFps;
private:
    Vehicule* monVehicule;
    Camera * camera;
    int framePerSecond;
protected:
    void run();
public slots:
    void sltTimerFpsTick();
signals:
    void sigSendCamImg(QPixmap img);
    void sigSendCamFps(int framePerSecond);
    void sigCamNotFound();
};

#endif // THREADCAM_H
