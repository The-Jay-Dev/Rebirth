#ifndef MAPDATA_H
#define MAPDATA_H
#include "EmotionalCapacityStates.h"
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVector>

class  MapData : public QObject{
    Q_OBJECT
public:


    explicit MapData(QObject *parent = nullptr): QObject(parent)
    {}

    int noteSpeed = 1;
    int trackDifficulty = 1;

    float trackBPM = 0.0;
    float beatOffset = 0.0;
    float previewStartTime = 60;
    float previewEndTime = 80;

    bool error = false;

    QUrl imageFilePath;
    QUrl particleFilePath;

    QString primaryColor = "white";
    QString secondaryColor = "red";
    QString audioFilePath = "";
    QString highlightColor = "white";
    QString particleColor = "white";
    QString trackTitle = "";
    QString trackArtist = "";
    QString mapIllustration = "";
    QString trackStyle = "Invoke";
    QString laneTheme = "rebirth";

    QVector<EmotionalCapacityStates> stateList;
};

#endif // MAPDATA_H
