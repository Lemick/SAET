#-------------------------------------------------
#
# Project created by QtCreator 2015-01-23T10:11:48
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt_IT1_SAET_SAS
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    alarme.cpp \
    camera.cpp \
    sas.cpp \
    vehicule.cpp \
    threadcam.cpp \
    panneau.cpp \
    panneaucirculaire.cpp \
    panneauagglomeration.cpp \
    Math.cpp \
    timer.cpp

HEADERS  += mainwindow.h \
    alarme.h \
    camera.h \
    sas.h \
    vehicule.h \
    asmOpenCV.h \
    threadcam.h \
    panneau.h \
    panneaucirculaire.h \
    panneauagglomeration.h \
    Math.h \
    timer.h

FORMS    += mainwindow.ui

INCLUDEPATH +=  ./3rdparty/include \

LIBS += -L"$$_PRO_FILE_PWD_/3rdparty/bin" \
    -L"$$_PRO_FILE_PWD_/3rdparty/lib" \
    -lopencv_core2410 \
    -lopencv_highgui2410 \
    -lopencv_imgproc2410 \
    -lopencv_features2d2410 \
    -lopencv_objdetect2410 \
    -lopencv_ml2410 \
    -lopencv_nonfree2410\
    -lopencv_flann2410 \



