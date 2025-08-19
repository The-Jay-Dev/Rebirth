#ifndef HITOBJECT_H
#define HITOBJECT_H
#include "HitObjectEnums.h"
#include <QObject>
#include <QPointer>
class HitObject : public QObject
{
    Q_OBJECT
public:


    explicit HitObject(QObject*parent = nullptr){};

    QSharedPointer<HitObject> tailObject;
    QSharedPointer<HitObject> headObject;
    HitObjectEnums::Types type = HitObjectEnums::Tap;
    HitObjectEnums::ColorTypes colorType = HitObjectEnums::QuarterNote;

    int lane;
    int tickSoundBeat = 0;
    int objectPoolIndex = 0;
    int totalTravelDistance = 0;
    double beat;

    float xPosition = 0.0;
    float yPosition = 0.0;
    float noteHoldDuration = 5;
    float laneXPos = 0.0;
    float tapDelayDuration = 1.0;
    float doubleTapMaxScale = 3.0;
    float secondTapBeatTime = 0.0;
    float secondTapJudgementScale = doubleTapMaxScale;

    bool good = false;
    bool holdActivated = false;
    bool holdFailed = false;
    bool missed = false;
    bool noteActive = false;
    bool okay = false;
    bool perfect = false;
    bool secondTapActive = false;
    bool tailNote = false;
    bool slideEventReady = false;
    bool visible = false;
    QString debugText;

};

#endif // HITOBJECT_H
