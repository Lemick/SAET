#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>


using std::cout;
using std::endl;


class Vehicule;
class Alarme;
class Camera;
class ThreadCam;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    ThreadCam* threadCamera;
    QPixmap qPixVoySurvitesse, qPixVoyBlack, qPixLogo;
    Vehicule* monVehicule;
    Alarme* monAlarme;
    Ui::MainWindow *ui;
    QTimer* timerRefreshVit;
    QTimer* timerFps;
    QTimer* timerLaunchedThread;
public slots:
    void sltReceiveCamImage(QPixmap img);
    void sltRefreshCompteur();
    void sltShowAlarmeVoyantTick();
    void sltHideAlarmeVoyantTick();
    void sltReceiveCamFps(int framePerSecond);
    void sltCamNotFound();
private slots:
    void on_cvPRondAvertVisu_clicked(bool checked);
    void on_cvPRondAvertSon_clicked(bool checked);
    void on_sliderCompteur_sliderMoved(int position);
};

#endif // MAINWINDOW_H
