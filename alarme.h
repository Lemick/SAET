#ifndef ALARME_H
#define ALARME_H

#include <QObject>
#include <QTimer>
#include <QSound>

class Vehicule;

class Alarme : public QObject
{
    Q_OBJECT

public:
    Alarme(Vehicule *leVehicule);
    void set_alrmSonore(bool isActived);
    void set_alrmVisuel(bool isActived);

private:
    QSound* alarme;
    QTimer* timerAlarmeTick;
    QTimer* timerAlarmeClign;
    bool alrmSonore, alrmVisuel; // Alarme relatives aux panneaux de limitations ronds
    bool alrmVisuelClign; // relatif au clignement de l'alarme
    Vehicule* monVehicule;
private slots:
    void sltMajAlarmes();
    void sltMajClign();
signals:
    void sigShowVoyantSurvitesse();
    void sigHideVoyantSurvitesse();

};

#endif // ALARME_H
