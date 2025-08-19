#ifndef TIMESTEP_H
#define TIMESTEP_H

#include <QElapsedTimer>
#include <QObject>

#include <QThread>
#include <QRunnable>
#include <QDebug>
#include <QTimer>
#include "GameInitializer.h"
class TimeStep : public QThread
{
    Q_OBJECT
public:

    double deltatime = 0.0;
    double refreshDelayMs = 1;

    QElapsedTimer runtimeTimer;

    GameInitializer* initializer;
    explicit TimeStep(GameInitializer* init = nullptr)
        : initializer(init)
    {}
    ~TimeStep(){}


    void run () override {

        QTimer* delay = new QTimer;
        QThread::currentThread()->setObjectName("TimeStep Thread");

        runtimeTimer.start();
        connect(delay, &QTimer::timeout, this, &TimeStep::timeStepTimeOut, Qt::QueuedConnection);
        delay->setTimerType (Qt::PreciseTimer);
        delay->start(refreshDelayMs);
        exec();
        delay = nullptr;

    }

public slots:
    void timeStepTimeOut(){
        deltatime = (double)runtimeTimer.nsecsElapsed()/1'000'000;
                    runtimeTimer.restart();
                    if(initializer)
                    {
                        initializer->tick(deltatime);
                    }
    }

signals:
    void tick(float deltatime);
};

#endif // TIMESTEP_H

