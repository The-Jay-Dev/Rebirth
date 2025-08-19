#include "DataHandlerModel.h"
#include "MapFileHandler.h"
#include "miniaudio.h"



DataHandlerModel::DataHandlerModel(QObject* parent, MessageHandler* msgHandler) : QAbstractListModel(parent), messageHandler(msgHandler){}


void DataHandlerModel::readMapFiles()
{
    MapFileHandler* mapFileHandler = new MapFileHandler(this, messageHandler);

    for (int i = 0; i <  mapFileHandler->fileNameList.length(); ++i) {
        QSharedPointer<MapData> obj = mapFileHandler->ParseMapFile(i);

        if(!obj->error){
            beginInsertRows(QModelIndex(), objects.size(), objects.size());
            objects.append(obj);
            endInsertRows();
        }
    }

    setMapCount(objects.size());
    setCurrentMapDetails();
    mapFileHandler = nullptr;


}

void DataHandlerModel::setCurrentMapDetails()
{
    if(currentMapIndex() < mapCount() && !objects.isEmpty()){
        currentMapObject = objects.at(currentMapIndex()); //Only hardcode for debugging. When debugging you must also index check to avoid crash
        setCurrentMapArtist(currentMapObject->trackArtist);

        audioFilePath = currentMapObject->audioFilePath;
        setCurrentMapBPM( currentMapObject->trackBPM);
        setCurrentMapDifficulty(currentMapObject->trackDifficulty);
        setCurrentMapImgPath(currentMapObject->imageFilePath);
        setCurrentMapStyle(currentMapObject->trackStyle);
        setCurrentMapTitle(currentMapObject->trackTitle);
        setCurrentMapIllustration(currentMapObject->mapIllustration);
        setHighlightColor(currentMapObject->highlightColor);
        setPrimaryColor(currentMapObject->primaryColor);
        setSecondaryColor(currentMapObject->secondaryColor);
        setParticleColor(currentMapObject->particleColor);
        setParticleFilePath(currentMapObject->particleFilePath);
        setLaneTheme(currentMapObject->laneTheme);
        setNoteSpeed(currentMapObject->noteSpeed);


        if (!gameStart() && !titleScreen())
            playTrackPreview(currentMapObject->previewStartTime, currentMapObject->previewEndTime);
    }
}

void DataHandlerModel::prepMapSelect()
{
    setGameStart(false);
    mapRefresh();
    emit dataRefreshed();
}

void DataHandlerModel::uninitializeMusic()
{
    //Before calling uninitializeMusic, ensure audioHandler->validSoundObject is true
    if( ma_sound_is_playing(&audioHandler->sound)){
        ma_sound_stop(&audioHandler->sound);
    }
    audioHandler->uninitializeMusic();


}

void DataHandlerModel::playTrackPreview(const int startTime, int const endTime)
{
    trackPreviewActivated = true;

    if(audioHandler && audioHandler->validSoundObject){
        uninitializeMusic();
        audioHandler->initializeMusic(audioFilePath);
        audioHandler->source = ma_sound_get_data_source(&audioHandler->sound);

        if (audioHandler->source)
            ma_data_source_set_loop_point_in_pcm_frames( audioHandler->source,
                                                        ma_uint64(startTime * ma_engine_get_device(&audioHandler->engine)->sampleRate),
                                                        ma_uint64(endTime * ma_engine_get_device(&audioHandler->engine)->sampleRate));

        ma_sound_set_looping(&audioHandler->sound, true);
        ma_sound_start(&audioHandler->sound);
        ma_sound_seek_to_pcm_frame(&audioHandler->sound, startTime * ma_engine_get_device(&audioHandler->engine)->sampleRate);
    }
    else{
        emit messageHandler->logMessage("Audio initialization failed \naudioHandler does not exist", MessageHandler::FAILED);

    }


}

void DataHandlerModel::attemptRandomTrackStart(int num) //Ran every few seconds by QML to keep the music running
{
    if(audioHandler){

        if(trackPreviewActivated ){
            if(audioHandler->validSoundObject && ma_sound_is_playing(&audioHandler->sound)){
                trackPreviewActivated = false;
                ma_sound_set_looping(&audioHandler->sound, false);
                emit titleTrackStarted(objects.at(currentMapIndex())->trackTitle);
            }
        }else
        {
            if(!objects.isEmpty() && !ma_sound_is_playing(&audioHandler->sound) && num < mapCount()){

                if(audioHandler->validSoundObject)
                    uninitializeMusic();

                audioHandler->initializeMusic(objects.at(num)->audioFilePath);
                ma_sound_start(&audioHandler->sound);
                emit titleTrackStarted(objects.at(num)->trackTitle);
            }
        }
    }
}

int DataHandlerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return objects.size();
}

QVariant DataHandlerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= objects.size())
        return QVariant();

    const QSharedPointer<MapData> mapObj = objects.at(index.row());
    switch (role) {
    case Title: return mapObj->trackTitle;
    case Artist: return mapObj->trackArtist;
    case Difficulty: return mapObj->trackDifficulty;
    case BPM: return mapObj->trackBPM;
    case Style: return mapObj->trackStyle;
    default: return QVariant(); //Safety net in case none of the above are returned
    }
}

QHash<int, QByteArray> DataHandlerModel::roleNames() const
{
    static QHash<int, QByteArray> info {
        {Title, "title"},
        {Artist, "artist"},
        {Difficulty, "difficulty"},
        {BPM, "bpm"},
        {Style, "style"}
    };
    return info;
}

QString DataHandlerModel::currentMapTitle() const
{
    return m_currentMapTitle;
}

void DataHandlerModel::setCurrentMapTitle(const QString &newCurrentMapTitle)
{
    if (m_currentMapTitle == newCurrentMapTitle)
        return;
    m_currentMapTitle = newCurrentMapTitle;
    emit currentMapTitleChanged();
}

QString DataHandlerModel::currentMapArtist() const
{
    return m_currentMapArtist;
}

void DataHandlerModel::setCurrentMapArtist(const QString &newCurrentMapArtist)
{
    if (m_currentMapArtist == newCurrentMapArtist)
        return;
    m_currentMapArtist = newCurrentMapArtist;
    emit currentMapArtistChanged();
}

int DataHandlerModel::currentMapBPM() const
{
    return m_currentMapBPM;
}

void DataHandlerModel::setCurrentMapBPM(int newCurrentMapBPM)
{
    if (m_currentMapBPM == newCurrentMapBPM)
        return;
    m_currentMapBPM = newCurrentMapBPM;
    emit currentMapBPMChanged();
}

int DataHandlerModel::currentMapDifficulty() const
{
    return m_currentMapDifficulty;
}

void DataHandlerModel::setCurrentMapDifficulty(int newCurrentMapDifficulty)
{
    if (m_currentMapDifficulty == newCurrentMapDifficulty)
        return;
    m_currentMapDifficulty = newCurrentMapDifficulty;
    emit currentMapDifficultyChanged();
}

QString DataHandlerModel::currentMapStyle() const
{
    return m_currentMapStyle;
}

void DataHandlerModel::setCurrentMapStyle(const QString &newCurrentMapStyle)
{
    if (m_currentMapStyle == newCurrentMapStyle)
        return;
    m_currentMapStyle = newCurrentMapStyle;
    emit currentMapStyleChanged();
}

int DataHandlerModel::currentMapIndex() const
{
    return m_currentMapIndex;
}

void DataHandlerModel::setCurrentMapIndex(int newCurrentMapIndex)
{
    if (m_currentMapIndex == newCurrentMapIndex)
        return;
    m_currentMapIndex = newCurrentMapIndex;
    emit currentMapIndexChanged();
}

QUrl DataHandlerModel::currentMapImgPath() const
{
    return m_currentMapImgPath;
}

void DataHandlerModel::setCurrentMapImgPath(const QUrl &newCurrentMapImgPath)
{
    if (m_currentMapImgPath == newCurrentMapImgPath)
        return;
    m_currentMapImgPath = newCurrentMapImgPath;
    emit currentMapImgPathChanged();
}

void DataHandlerModel::mapRefresh()
{
    beginResetModel();
    objects.clear();
    endResetModel();
    readMapFiles();
}

QString DataHandlerModel::primaryColor() const
{
    return m_primaryColor;
}

void DataHandlerModel::setPrimaryColor(const QString &newPrimaryColor)
{
    if (m_primaryColor == newPrimaryColor)
        return;
    m_primaryColor = newPrimaryColor;
    emit primaryColorChanged();
}

QString DataHandlerModel::secondaryColor() const
{
    return m_secondaryColor;
}

void DataHandlerModel::setSecondaryColor(const QString &newSecondaryColor)
{
    if (m_secondaryColor == newSecondaryColor)
        return;
    m_secondaryColor = newSecondaryColor;
    emit secondaryColorChanged();
}

QString DataHandlerModel::highlightColor() const
{
    return m_highlightColor;
}

void DataHandlerModel::setHighlightColor(const QString &newHighlightColor)
{
    if (m_highlightColor == newHighlightColor)
        return;
    m_highlightColor = newHighlightColor;
    emit highlightColorChanged();
}

QString DataHandlerModel::particleColor() const
{
    return m_particleColor;
}

void DataHandlerModel::setParticleColor(const QString &newParticleColor)
{
    if (m_particleColor == newParticleColor)
        return;
    m_particleColor = newParticleColor;
    emit particleColorChanged();
}

QUrl DataHandlerModel::particleFilePath() const
{
    return m_particleFilePath;
}

void DataHandlerModel::setParticleFilePath(const QUrl &newParticleFilePath)
{
    if (m_particleFilePath == newParticleFilePath)
        return;
    m_particleFilePath = newParticleFilePath;
    emit particleFilePathChanged();
}

int DataHandlerModel::noteSpeed() const
{
    return m_noteSpeed;
}

void DataHandlerModel::setNoteSpeed(int newNoteSpeed)
{
    if (m_noteSpeed == newNoteSpeed)
        return;
    m_noteSpeed = newNoteSpeed;
    emit noteSpeedChanged();
}

QString DataHandlerModel::laneTheme() const
{
    return m_laneTheme;
}

void DataHandlerModel::setLaneTheme(const QString &newLaneTheme)
{
    if (m_laneTheme == newLaneTheme)
        return;
    m_laneTheme = newLaneTheme;
    emit laneThemeChanged();
}

bool DataHandlerModel::gameStart() const
{
    return m_gameStart;
}

void DataHandlerModel::setGameStart(bool newGameStart)
{
    if (m_gameStart == newGameStart)
        return;
    m_gameStart = newGameStart;
    emit gameStartChanged();
}

bool DataHandlerModel::titleScreen() const
{
    return m_titleScreen;
}

void DataHandlerModel::setTitleScreen(bool newTitleScreen)
{
    if (m_titleScreen == newTitleScreen)
        return;
    m_titleScreen = newTitleScreen;
    emit titleScreenChanged();
}

int DataHandlerModel::mapCount() const
{
    return m_mapCount;
}

void DataHandlerModel::setMapCount(int newMapCount)
{
    if (m_mapCount == newMapCount)
        return;
    m_mapCount = newMapCount;
    emit mapCountChanged();
}

QString DataHandlerModel::currentMapIllustration() const
{
    return m_currentMapIllustration;
}

void DataHandlerModel::setCurrentMapIllustration(const QString &newCurrentMapIllustration)
{
    if (m_currentMapIllustration == newCurrentMapIllustration)
        return;
    m_currentMapIllustration = newCurrentMapIllustration;
    emit currentMapIllustrationChanged();
}
