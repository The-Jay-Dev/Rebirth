#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QObject>
#include "miniaudio.h"
#include "MessageHandler.h"

class AudioHandler : public QObject
{
    Q_OBJECT
public:
   explicit AudioHandler(QObject* parent = nullptr,  MessageHandler* msgHandler = nullptr);
    ~AudioHandler();

    ma_engine engine;
    MessageHandler* messageHandler;
    ma_sound sound;
    ma_sound hitSoundA;
    ma_sound hitSoundB;
    ma_sound hitSoundC;
    ma_sound hitSoundD;
    ma_result engineResult;
    ma_engine_config engineConfig;
    ma_data_source* source;
    bool validSoundObject = false;
    bool validEngineObject = false;
    bool debug = false;
    void initializeMusic(const QString path);

    void initializeEngine();
    void reInstallEngine();
    void initializeGroups();
    void uninitializeMusic();
    void initializeSoundEffects();
    const char* stringToCharPointer(const QString path);
signals:
    void initializationError(QString text);
    void initializationSuccess(QString text);  
};

#endif // AUDIOHANDLER_H
