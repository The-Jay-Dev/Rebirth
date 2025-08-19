#ifndef MAPFILEHANDLER_H
#define MAPFILEHANDLER_H

#include "EmotionalCapacityStates.h"
#include "MapData.h"
#include "MessageHandler.h"
#include "Notes.h"
#include <QStringListModel>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QUrl>

class MapFileHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl imageFilePath READ imageFilePath WRITE setImageFilePath NOTIFY imageFilePathChanged FINAL)
    Q_PROPERTY(QUrl particleFilePath READ particleFilePath WRITE setParticleFilePath NOTIFY particleFilePathChanged FINAL)
    Q_PROPERTY(QString primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged FINAL)
    Q_PROPERTY(QString secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged FINAL)
    Q_PROPERTY(QString highlightColor READ highlightColor WRITE setHighlightColor NOTIFY highlightColorChanged FINAL)
    Q_PROPERTY(QString particleColor READ particleColor WRITE setParticleColor NOTIFY particleColorChanged FINAL)
public:
    explicit MapFileHandler(QObject *musicConductor = nullptr, MessageHandler* msgHandler = nullptr);

     QSharedPointer<MapData> ParseMapFile(int trackNum = 0);
    MessageHandler* messageHandler;
    int trackDifficulty = 0;
    float bpm = 0;
    float beatOffset = 0;

    QString trackTitle = "";
    QString trackArtist = "";
    QString laneTheme = "";
    QString mapIllustration = "";

    QVector<Note> notes;
    enum capacityStates{
        Euphoria,
        Nostalgia,
        Paradox,
        Confusion,
        Fracture
    };
    QVector <EmotionalCapacityStates> stateList;
    QString audioFilePath = "";



    QUrl imageFilePath() const;
    void setImageFilePath(const QUrl &newImageFilePath);

    QString secondaryColor() const;
    void setSecondaryColor(const QString &newSecondaryColor);

    QString primaryColor() const;
    void setPrimaryColor(const QString &newPrimaryColor);

    QString highlightColor() const;
    void setHighlightColor(const QString &newHighlightColor);

    QUrl particleFilePath() const;
    void setParticleFilePath(const QUrl &newParticleFilePath);

    QString particleColor() const;
    void setParticleColor(const QString &newParticleColor);

    QStringList fileNameList;

signals:

    void fileNameListUpdated();

    void filesUpdated();

    void filesModelUpdated();

    void imageFilePathChanged();

    void secondaryColorChanged();

    void primaryColorChanged();

    void highlightColorChanged();

    void particleFilePathChanged();

    void particleColorChanged();

    void imgDimPrimaryColorChanged();

    void brightImgDimSecondaryColorChanged();

private:
    QDir mapFolderDir;
    QDir particleFolderDir;

    QStringList fileList;
    QStringList filters;
    QStringListModel *fileListModel = new QStringListModel(this);


    QString determineStyle(const QVector <EmotionalCapacityStates> &stateList);
    QString GetTrackName(int trackNum = 0) const;

    float previewStartTime = 60;
    float previewEndTime = 80;

    int noteSpeed = 1;

    void updateFileListModel(QStringListModel *newFileListModel);
    void updateFileList(const QStringList &newFileList);
    void UpdateFileNameList();
    bool UpdateVariables(QJsonDocument *document);
    bool initializeEmotionalCapacity(QJsonDocument* document, QSharedPointer<MapData> data);
    void initializeMapData(QSharedPointer<MapData> data);

    QUrl m_imageFilePath;
    QUrl m_particleFilePath;
    QString m_secondaryColor = ""; //currently unused
    QString m_primaryColor = "";
    QString m_highlightColor = "";
    QString m_particleColor = "";
};

#endif // MAPFILEHANDLER_H
