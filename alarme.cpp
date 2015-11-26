#include "alarme.h"
#include "vehicule.h"

Alarme::Alarme(Vehicule* leVehicule)
{
    monVehicule = leVehicule;

    alarme = new QSound("./data/sounds/alarme.wav");
    alarme->setLoops(QSound::Infinite);

    alrmSonore = false;
    alrmVisuel = false;

    timerAlarmeTick = new QTimer(this);
    connect(timerAlarmeTick, SIGNAL(timeout()), this, SLOT(sltMajAlarmes()));
    timerAlarmeTick->start(50);

    timerAlarmeClign = new QTimer(this);
    connect(timerAlarmeClign, SIGNAL(timeout()), this, SLOT(sltMajClign()));
}

void Alarme::sltMajClign()
{
    alrmVisuelClign = !alrmVisuelClign;
    timerAlarmeClign->start(500);
}

void Alarme::set_alrmSonore(bool isActived)
{
    alrmSonore = isActived;
}

void Alarme::set_alrmVisuel(bool isActived)
{
    alrmVisuel = isActived;
    if (!timerAlarmeClign->isActive() && isActived)
    {
        timerAlarmeClign->start(500);
    }
}

void Alarme::sltMajAlarmes()
{
    if((monVehicule->get_limiteAutorise() != 0)    &&    (monVehicule->get_compteur() > monVehicule->get_limiteAutorise() ))
    {
        if (alrmSonore)
        {
            if(alarme->isFinished())
                alarme->play();
        }
        else
        {
            if(!alarme->isFinished())
                alarme->stop();
        }

        if (alrmVisuel)
            if(alrmVisuelClign)
            {
                emit sigShowVoyantSurvitesse();
            }
            else
            {
                emit sigHideVoyantSurvitesse();
            }
        else
        {
            emit sigHideVoyantSurvitesse();
            timerAlarmeClign->stop();
        }
    }

    if(monVehicule->get_compteur() < monVehicule->get_limiteAutorise() )
    {
        if (alrmVisuel)
            emit sigHideVoyantSurvitesse();

        if(alrmSonore)
            alarme->stop();
    }
}
