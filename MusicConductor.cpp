#include "MusicConductor.h"
#include "miniaudio.h"
#include <QThread>


void MusicConductor::gameStart()
{
    QThread::currentThread()->setObjectName("MusicConductor Thread");
    float delay = dataHandler->noteSpeed();
    float offset = dataHandler->currentMapObject->beatOffset;

    songStarted = false;
    offsetRemaining = true;
    delay += 2;

    if (elapsedTimer){
       delete elapsedTimer;
      elapsedTimer = nullptr;
    }

    elapsedTimer = new QElapsedTimer();
    if (dataHandler){
        secsPerBeat = 60/dataHandler->currentMapObject->trackBPM;
    }

    trackDelayTime = delay * secsPerBeat;
    trackOffsetTime = offset * secsPerBeat;
    totalOffsetTime = trackDelayTime + trackOffsetTime;
    totalOffsetBeats = delay + offset;
    soundOffsetBeats = offset;
    trackDelayBeats = delay;

    elapsedTimer->start();
}

void MusicConductor::getBeat()
{
    if (lastBeat < songPosInBeats)
    {
        if (currentMeasure > measures)
            currentMeasure = 1;
        lastBeat = songPosInBeats;
        currentMeasure += 1;
    }
}

//Make sure to provide an accurate offset for exact syncing
void MusicConductor::playFromBeat(float beat)
{
    if (elapsedTimer && audioHandler->validSoundObject && audioHandler->validEngineObject){
        playFromBeatActive = true;
        beatsSkipped = beat;
        ma_sound_start(&audioHandler->sound);

        offsetTime = totalOffsetTime;
        float soundBeatPosition = beat + soundOffsetBeats;
        float skipSoundFrames = soundBeatPosition * secsPerBeat * (float)ma_engine_get_device(&audioHandler->engine)->sampleRate;
        float engineBeatPosition = beat + totalOffsetBeats;
        float skipTrackFrames = engineBeatPosition * secsPerBeat * (float)ma_engine_get_device(&audioHandler->engine)->sampleRate;
        ma_sound_seek_to_pcm_frame( &audioHandler->sound, skipSoundFrames);
        engineSkippedTimeMs = (skipTrackFrames / (float)ma_engine_get_device(&audioHandler->engine)->sampleRate) * 1000.0;

        currentMeasure = std::fmod(beat, measures);
        secsSkipped = engineBeatPosition * secsPerBeat;

        songStarted = true;
        offsetRemaining = false;
        currentTime = skipTrackFrames/(double)ma_engine_get_sample_rate(&audioHandler->engine);
    }
}

void MusicConductor::tick(const float deltaTime)
{
    if (elapsedTimer && audioHandler->validSoundObject && audioHandler->validEngineObject){
        isMusicPlaying = ma_sound_is_playing(&audioHandler->sound);
        currentTime = ((double)elapsedTimer->elapsed() + (engineSkippedTimeMs))/ 1000;

        if (!playFromBeatActive && !songStarted && currentTime >= trackDelayTime){
            ma_sound_start(&audioHandler->sound);// have this come first to avoid conflicts with gameManager
            songStarted = true;

        }
        else if (!playFromBeatActive &&songStarted && offsetRemaining && currentTime >= totalOffsetTime){
            offsetRemaining = false;
            offsetTime = currentTime;
        }

        else if (songStarted && !offsetRemaining){

            songPos = currentTime - offsetTime;
            double outputLatency = (float)ma_engine_get_device(&audioHandler->engine)->playback.internalPeriodSizeInFrames/(float)ma_engine_get_sample_rate(&audioHandler->engine);
            songPos -= outputLatency;
            songPosInBeatsUnfiltered = (songPos/secsPerBeat);
            songPosInBeats = int(songPos/secsPerBeat);
            getBeat();
        }

    }
}


MusicConductor::~MusicConductor(){

    if (elapsedTimer){
        delete elapsedTimer;
        elapsedTimer = nullptr;
    }
}
