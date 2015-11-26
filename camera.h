#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "asmOpenCV.h"

#include <iostream>
#include <QSettings>


using namespace cv;
using std::cout;
using std::endl;

class Camera
{
public:
    Camera(bool& isSuccessfully);
    int readCamera_c(Mat& frame);
    int readCamera(Mat& frame);
    void resizeFrame(Mat& frame, double ratio);
    QString loadCamAddress();
private:
    CvCapture* capture;
    VideoCapture* vcap;
};

#endif // CAMERA_H
