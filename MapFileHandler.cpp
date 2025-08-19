#include "MapFileHandler.h"
#include "MessageHandler.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>


MapFileHandler::MapFileHandler(QObject *parent,  MessageHandler* msgHandler ): QObject(parent), messageHandler(msgHandler) {

    filters << "*.rgmap";
    mapFolderDir = QDir(QCoreApplication::applicationDirPath() + "/Tracks");
    particleFolderDir = QDir(QCoreApplication::applicationDirPath() + "/Particles");
    fileListModel->setStringList(mapFolderDir.entryList(filters, QDir::Files));

    updateFileListModel(fileListModel);
    updateFileList(fileListModel->stringList());
    UpdateFileNameList();

}

QString MapFileHandler::GetTrackName(int trackNum) const
{
    return trackNum > -1 ? fileNameList[trackNum] : "Error 404";
}

QSharedPointer<MapData> MapFileHandler::ParseMapFile(int trackNum)
{
    if (mapFolderDir.exists()){
        QSharedPointer<MapData> data =  QSharedPointer<MapData>(new MapData);
        QString chartPath = mapFolderDir.path() + "/" + GetTrackName(trackNum) + ".rgmap";
        QFile rgFile(chartPath);

        if (rgFile.open(QIODevice::ReadOnly)){

            QByteArray Bytes = rgFile.readAll(); //gather the data
            rgFile.close();

            QJsonParseError SyntaxError;
            //Convert to Json
            QJsonDocument Document = QJsonDocument::fromJson(Bytes, &SyntaxError);
            if (SyntaxError.error != QJsonParseError::NoError){
                emit messageHandler->logMessage("JSON PARSE ERROR/n" + SyntaxError.errorString() + "\nPlease check .rgmap",MessageHandler::FAILED);
            }

            /*The contents of the .rgmap file should be within a single Json object
         which allows us the condition to be true
        */
            if (Document.isObject()){

                data->error = UpdateVariables(&Document);
                data->error = initializeEmotionalCapacity(&Document, data);
                initializeMapData(data);
                stateList.clear();
                return data;

            }
        }

        data->error = true;
        emit  messageHandler->logMessage("Map data file invalid.\nPlease check that file uses the rgmap extension and follows the map file guidelines", MessageHandler::FAILED);
        return data;
    }
    else
        emit  messageHandler->logMessage("Failed to locate maps directory.", MessageHandler::FAILED);

    return {};

}

void MapFileHandler::UpdateFileNameList()
{
    for(int i = 0; i < fileList.length(); i++ ){
        int dotIndex = fileList[i].lastIndexOf(".rgmap");
        fileNameList << fileList[i].left(dotIndex);
    }

    emit fileNameListUpdated();
}

bool MapFileHandler::UpdateVariables(QJsonDocument* document)
{
    if(document->object().contains("title") && document->object().contains("artist")){
        trackTitle = document->object().value("title").toString();
        trackArtist = document->object().value("artist").toString();
        bpm = document->object().value("bpm").toDouble();
        beatOffset = document->object().value("beatOffset").toDouble();
        noteSpeed = document->object().value("noteSpeed").toInt();
        previewStartTime = document->object().value("previewStartTime").toDouble();
        trackDifficulty = document->object().value("difficulty").toInt();
        previewEndTime = document->object().value("previewEndTime").toDouble();
        setPrimaryColor(document->object().value("primaryColor").toString());

        if(document->object().contains("secondaryColor"))
            setSecondaryColor(document->object().value("secondaryColor").toString()); //currently unused

        setHighlightColor(document->object().value("highlightColor").toString());
        setParticleColor(document->object().value("particleColor").toString());
        laneTheme = document->object().value("laneTheme").toString();
        setImageFilePath( QUrl::fromLocalFile(mapFolderDir.path()+ "/" + document->object().value("imageFileName").toString()));
        setParticleFilePath( QUrl::fromLocalFile(particleFolderDir.path() + "/" + document->object().value("particleFileName").toString()));
        audioFilePath = mapFolderDir.path() + "/" + document->object().value("audioFile").toString();
        mapIllustration = document->object().value("illustration").toString();
        return false;

    }

        emit messageHandler->logMessage("Failed to initialize map data.\nObject requires both a title and an artist.", MessageHandler::FAILED);
        return true;


}

bool MapFileHandler::initializeEmotionalCapacity(QJsonDocument* document, QSharedPointer<MapData> data)
{
    const QJsonArray emotionalCapacityStates = document->object().value("emotionalCapacity").toArray();

    if(!emotionalCapacityStates.isEmpty()){

        for(const QJsonValue &state: emotionalCapacityStates){
            const QJsonObject stateListObj = state.toObject(); //declare as const to avoid range-loop warning
            const  QStringList keyList = stateListObj.keys();
            for (const QString &key : keyList){
                QJsonObject stateObj = stateListObj[key].toObject(); //Key holds the state name (i.e. Euphoria)


                if (stateObj.contains("notes")){
                    EmotionalCapacityStates currentState;
                    currentState.state = key;
                    const QJsonArray notesArray = stateObj.value("notes").toArray();

                    if(!notesArray.isEmpty()){

                        for(const QJsonValue &value: notesArray){
                            QJsonObject noteObj = value.toObject();
                            Note note;

                            note.beat = noteObj.value("beat").toDouble();
                            note.lane = noteObj.value("lane").toInt();

                            QString typeString =  noteObj.value("type").toString();

                            if (typeString == "tap")
                                note.type = HitObjectEnums::Tap;
                            else if (typeString == "doubleTap")
                                note.type = HitObjectEnums::DoubleTap;
                            else if (typeString == "hold")
                                note.type = HitObjectEnums::Hold;
                            else if (typeString == "slide")
                                note.type = HitObjectEnums::Slide;
                            else if (typeString == "forgotten")
                                note.type = HitObjectEnums::Forgotten;
                            else
                                note.type = HitObjectEnums::Tap;

                            if(noteObj.contains("duration"))
                                note.duration = noteObj.value("duration").toDouble();

                            if(noteObj.contains("tailLane"))
                                note.tailLane = noteObj.value("tailLane").toInt();
                            currentState.notes.push_back(note);
                        }


                        stateList.push_back(currentState);
                    }
                    else{
                         emit messageHandler->logMessage("Failed to Notes in " + trackTitle + ".\nObject may be empty.", MessageHandler::FAILED);
                        return true;
                    }


                }

            }
        }

        return false;
    }

    emit messageHandler->logMessage("Failed to initialize Emotional Capacity object in " + trackTitle + ".\nObject may be empty.", MessageHandler::FAILED);
    return true;



}

void MapFileHandler::initializeMapData(QSharedPointer<MapData> data)
{
    data->beatOffset = beatOffset;
    data->trackBPM = bpm;
    data->trackArtist = trackArtist;
    data->trackTitle = trackTitle;
    data->highlightColor = highlightColor();
    data->secondaryColor = secondaryColor();
    data->primaryColor = primaryColor();
    data->particleColor = particleColor();
    data->imageFilePath = imageFilePath();
    data->mapIllustration = mapIllustration;

    QDir dir;
    if (dir.exists(particleFilePath().toLocalFile()))
    {
        data->particleFilePath = particleFilePath();
    }
    else
        data->particleFilePath = QString();

    data->primaryColor = primaryColor();
    data->secondaryColor = secondaryColor();
    data->highlightColor = highlightColor();
    data->previewStartTime = previewStartTime;
    data->previewEndTime = previewEndTime;
    data->audioFilePath = audioFilePath;
    data->noteSpeed = noteSpeed;
    data->stateList = stateList;
    data->laneTheme = laneTheme;
    data->trackDifficulty = trackDifficulty;
    data->trackStyle = determineStyle(stateList);
}

QString MapFileHandler::determineStyle(const QVector <EmotionalCapacityStates> &stateList)
{
    QVector <QString> evoke;
    QVector <QString> invoke;

    for (const EmotionalCapacityStates &state : stateList){
        if (state.state == "Euphoria" || state.state == "Bliss"  || state.state == "Nostalgia" || state.state == "Confusion" || state.state == "Paradox" )
        {
            evoke.append(state.state);
        }
        else if (state.state == "Lapse" || state.state == "Reflection"  || state.state == "Retrieval" || state.state == "Fracture"){
            invoke.append(state.state);
        }
    }

    if (qMax(invoke.length(),evoke.length()) == invoke.length()){
        return "Invoke";
    }
    else
    {
        return "Evoke";
    }

}

QUrl MapFileHandler::imageFilePath() const
{
    return m_imageFilePath;
}

void MapFileHandler::setImageFilePath(const QUrl &newImageFilePath)
{
    if (m_imageFilePath == newImageFilePath)
        return;
    m_imageFilePath = newImageFilePath;
    emit imageFilePathChanged();
}

QString MapFileHandler::secondaryColor() const
{
    return m_secondaryColor;
}

void MapFileHandler::setSecondaryColor(const QString &newSecondaryColor)
{
    if (m_secondaryColor == newSecondaryColor)
        return;
    m_secondaryColor = newSecondaryColor;
    emit secondaryColorChanged();
}

QString MapFileHandler::primaryColor() const
{
    return m_primaryColor;
}

void MapFileHandler::setPrimaryColor(const QString &newPrimaryColor)
{
    if (m_primaryColor == newPrimaryColor)
        return;
    m_primaryColor = newPrimaryColor;
    emit primaryColorChanged();
}


QString MapFileHandler::highlightColor() const
{
    return m_highlightColor;
}

void MapFileHandler::setHighlightColor(const QString &newHighlightColor)
{
    if (m_highlightColor == newHighlightColor)
        return;
    m_highlightColor = newHighlightColor;
    emit highlightColorChanged();
}

QUrl MapFileHandler::particleFilePath() const
{
    return m_particleFilePath;
}

void MapFileHandler::setParticleFilePath(const QUrl &newParticleFilePath)
{
    if (m_particleFilePath == newParticleFilePath)
        return;
    m_particleFilePath = newParticleFilePath;
    emit particleFilePathChanged();
}

QString MapFileHandler::particleColor() const
{
    return m_particleColor;
}

void MapFileHandler::setParticleColor(const QString &newParticleColor)
{
    if (m_particleColor == newParticleColor)
        return;
    m_particleColor = newParticleColor;
    emit particleColorChanged();
}

void MapFileHandler::updateFileListModel(QStringListModel *newFileListModel)
{
    if(fileListModel == newFileListModel)
        return;
    fileListModel = newFileListModel;
    emit filesModelUpdated();
}

void MapFileHandler::updateFileList(const QStringList &newFileList)
{
    if(fileList == newFileList)
        return;
    fileList = newFileList;
    emit filesUpdated();
}
