#include "AudioHandler.h"
#include <QCoreApplication>
#include <QDir>

AudioHandler::AudioHandler(QObject* parent, MessageHandler* msgHandler): QObject(parent), messageHandler(msgHandler), sound{}
{}

void AudioHandler::initializeMusic(const QString path)
{

    ma_result soundResult = ma_sound_init_from_file(&engine, stringToCharPointer(path), 0, 0, NULL, &sound);
    if(soundResult != MA_SUCCESS)
    {
        validSoundObject = false;
         emit messageHandler->logMessage("Audio initialization failed on path:\n" + path, MessageHandler::FAILED);
    }
    else{
        validSoundObject = true;
        ma_sound_set_looping(&sound, false);
        ma_sound_seek_to_pcm_frame(&sound,0);
        ma_sound_set_volume(&sound, 0.6);

        if(debug){
             emit messageHandler->logMessage("Audio initialization succeeded on path:\n" + path, MessageHandler::FAILED);
        }

    }
}

void AudioHandler::initializeSoundEffects()
{

    QDir soundsFolderDir = QDir(QCoreApplication::applicationDirPath() + "/Sounds");

    if(soundsFolderDir.exists()){

        QString hitSoundAPath = soundsFolderDir.path() +  "/RebirthHitsoundA.wav";
        QString hitSoundAReverbPath = soundsFolderDir.path() +  "/RebirthHitsoundAReverb.wav";
        QString hitSoundForgottenPath = soundsFolderDir.path() +  "/RebirthHitsoundForgotten.wav";
        QString tickAPath = soundsFolderDir.path() + "/RebirthHitsoundTickA.wav";

        ma_result soundResultA = ma_sound_init_from_file(&engine, stringToCharPointer(hitSoundAPath), 0, NULL, NULL, &hitSoundA);
        ma_result soundResultB = ma_sound_init_from_file(&engine, stringToCharPointer(hitSoundAReverbPath), 0, NULL, NULL, &hitSoundB);
        ma_result soundResultC = ma_sound_init_from_file(&engine, stringToCharPointer(tickAPath), 0, NULL, NULL, &hitSoundC);
        ma_result soundResultD = ma_sound_init_from_file(&engine, stringToCharPointer(hitSoundForgottenPath), 0, NULL, NULL, &hitSoundD);

        if(soundResultA != MA_SUCCESS)
        {
            emit messageHandler->logMessage("Hitsound Audio initialization failed on path:\n" + hitSoundAPath, MessageHandler::FAILED);
        }
        else if(soundResultB != MA_SUCCESS)
        {
            emit messageHandler->logMessage("Hitsound Audio initialization failed on path:\n" + hitSoundAReverbPath, MessageHandler::FAILED);
        }
        else if(soundResultC != MA_SUCCESS)
        {
             emit messageHandler->logMessage("Hitsound Audio initialization failed on path:\n" + tickAPath, MessageHandler::FAILED);
        }
        else if(soundResultD != MA_SUCCESS)
        {
            emit messageHandler->logMessage("Hitsound Audio initialization failed on path:\n" + tickAPath, MessageHandler::FAILED);
        }

    }
    else{
          emit messageHandler->logMessage("Missing sounds directory at:\n" + soundsFolderDir.path(), MessageHandler::FAILED);
    }
}

void AudioHandler::uninitializeMusic()
{
    ma_sound_uninit(&sound);
    validSoundObject = false;
}

const char* AudioHandler::stringToCharPointer(const QString path)
{
        //ma_sound_init_from_file expects a const char*, so we convert
    QByteArray byteArray = path.toUtf8();
    const char* audioPath = byteArray.constData();
    return audioPath;
}

AudioHandler::~AudioHandler(){

    ma_sound_uninit(&hitSoundA);
    ma_sound_uninit(&hitSoundB);
    ma_sound_uninit(&hitSoundC);
    ma_sound_uninit(&hitSoundD);
    ma_sound_uninit(&sound);
    ma_data_source_uninit(&source);
    ma_engine_uninit(&engine);
}

void AudioHandler::initializeEngine()
{
    engineConfig = ma_engine_config_init();
    engineConfig.periodSizeInMilliseconds = 10;  //periodSize = latency (10ms is a reasonable default to attempt)
    engineResult = ma_engine_init(&engineConfig, &engine);

    if(engineResult != MA_SUCCESS){
        validEngineObject = false;
    }
    else
    {
        validEngineObject = true;
    }
}

void AudioHandler::reInstallEngine()
{
    validEngineObject = false;
    ma_engine_uninit(&engine);
    initializeEngine();
}


