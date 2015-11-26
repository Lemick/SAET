#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "camera.h"
#include "vehicule.h"
#include "alarme.h"
#include "threadcam.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    qPixVoyBlack = QPixmap("./data/img/voyantBlack.png");
    qPixVoySurvitesse = QPixmap("./data/img/newVoyOrange.png");
    qPixLogo = QPixmap("./data/img/logo_SAET.png");

    /*
    if(qPixVoyBlack.isNull() || qPixVoySurvitesse.isNull())
    {
        ui->statusBar->showMessage("ERROR: data/img  non charge, verifiez que le dossier est bien a la racine du repertoire de compilation. Arret..",5000);
        cout << "ERROR: data/img  non charge, verifiez que le dossier est bien a la racine du repertoire de compilation. Arret.." <<endl;
    }
    else
    */

    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    ui->lablVoyantSurvitesse->setPixmap(qPixVoyBlack);
    ui->lablLogo->setPixmap(qPixLogo);
    monVehicule = new Vehicule(monAlarme);
    timerRefreshVit = new QTimer(this);

    connect(timerRefreshVit, SIGNAL(timeout()),this , SLOT(sltRefreshCompteur()));

    connect(monAlarme , SIGNAL(sigShowVoyantSurvitesse()), this, SLOT(sltShowAlarmeVoyantTick()));
    connect(monAlarme , SIGNAL(sigHideVoyantSurvitesse()), this, SLOT(sltHideAlarmeVoyantTick()));

    // Creation du thread et connexions des slots camera et nb d'images secondes
    threadCamera = new ThreadCam(monVehicule);

    connect(threadCamera,SIGNAL(sigSendCamImg(QPixmap)),this,SLOT(sltReceiveCamImage(QPixmap)));
    connect(threadCamera,SIGNAL(sigSendCamFps(int)),this,SLOT(sltReceiveCamFps(int)));
    connect(threadCamera, SIGNAL(sigCamNotFound()),this , SLOT(sltCamNotFound()));

    timerRefreshVit->start(250);
    ui->lablFrame->setText("Recherche du flux cours...");
    threadCamera->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

///Verifie si le thread est lancé et donc que la caméra est detectée
void MainWindow::sltCamNotFound()
{
    ui->lablFrame->setText("Le flux vidéo n'a put être trouvé.\n Verifiez l'adresse du flux dans le fichier data/config/config.ini");
}

///Affiche l'image de la caméra traité par le SAS
void  MainWindow::sltReceiveCamImage(QPixmap imgRecue)
{
    ui->lablFrame->setPixmap(imgRecue);
}

///Affiche la limitation de vitesse
void MainWindow::sltRefreshCompteur()
{

    ui->lablLimitationActuelle->setText(QString::number(monVehicule->get_limiteAutorise()));
}

void MainWindow::on_cvPRondAvertVisu_clicked(bool checked)
{
    monAlarme->set_alrmVisuel(checked);
}

void MainWindow::on_cvPRondAvertSon_clicked(bool checked)
{
    monAlarme->set_alrmSonore(checked);
}

void MainWindow::sltShowAlarmeVoyantTick()
{
    ui->lablVoyantSurvitesse->setPixmap(qPixVoySurvitesse);
}

void MainWindow::sltHideAlarmeVoyantTick()
{
    ui->lablVoyantSurvitesse->setPixmap(qPixVoyBlack);
}

///Affiche le nombre d'images par seconde
void MainWindow::sltReceiveCamFps(int framePerSecond)
{
    ui->statusBar->showMessage("Frames par seconde : " + QString::number(framePerSecond));
}

void MainWindow::on_sliderCompteur_sliderMoved(int position)
{
    ui->lcdCompteurVit->display(position);
    monVehicule->set_compteur(position);
}

