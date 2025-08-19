#ifndef MUSICCONDUCTOR_H
#define MUSICCONDUCTOR_H

#include "AudioHandler.h"
#include "DataHandlerModel.h"
#include "miniaudio.h"
#include <QObject>
#include <QElapsedTimer>
#include <chrono>

class MapFileHandler;

class MusicConductor: public QObject
{
    Q_OBJECT

public:
    MusicConductor(QObject* parent = nullptr, AudioHandler* audioHandlerObj = nullptr):QObject(parent), audioHandler(audioHandlerObj)
    {}

    ~MusicConductor() override;

    int currentMeasure = 1;
    int beatOffset = 0;

    double currentTime = 0.0;
    double offsetTime = 0.0;
    double secsPerBeat = 0.0;
    double songPos = 0.0;
    double songPosInBeatsUnfiltered = 0.0;

    float offset = 0.2;
    float lastBeat = 0.0;
    float secsSkipped = 0.0;
    float totalOffsetBeats = 0.0;
    float beatsSkipped = 0.0;
    float songPosInBeats = 0.0;
    float cursor = 0.0;
    float lastAudioTime = 0.0;

    bool songStarted = false;
    bool playFromBeatActive = false;
    bool offsetRemaining = true;

    qint32 measures = 4;
    ma_int64 offsetFrames = 0;
    ma_bool32 isMusicPlaying = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    AudioHandler* audioHandler;

    QElapsedTimer *elapsedTimer = nullptr;
    DataHandlerModel* dataHandler;

    void gameStart();
    void addBeatOffset(int offset);
    void playFromBeat(float beat);
    void getBeat();
public slots:
    void tick(const float deltaTime);
private:
    float trackOffsetTime = 0.0;
    float trackDelayTime = 0.0;
    float totalOffsetTime = 0.0;
    float soundOffsetBeats = 0.0;
    float trackDelayBeats = 0.0;
    float engineSkippedTimeMs =0.0;
};

#endif // MUSICCONDUCTOR_H
