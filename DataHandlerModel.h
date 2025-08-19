#ifndef DATAHANDLERMODEL_H
#define DATAHANDLERMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include "MapData.h"
#include "miniaudio.h"
#include "AudioHandler.h"
#include "MessageHandler.h"

class DataHandlerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentMapTitle READ currentMapTitle WRITE setCurrentMapTitle NOTIFY currentMapTitleChanged FINAL)
    Q_PROPERTY(QString currentMapArtist READ currentMapArtist WRITE setCurrentMapArtist NOTIFY currentMapArtistChanged FINAL)
    Q_PROPERTY(int currentMapBPM READ currentMapBPM  WRITE setCurrentMapBPM  NOTIFY currentMapBPMChanged FINAL)
    Q_PROPERTY(int currentMapDifficulty READ currentMapDifficulty WRITE setCurrentMapDifficulty NOTIFY currentMapDifficultyChanged FINAL)
    Q_PROPERTY(QString currentMapStyle READ currentMapStyle WRITE setCurrentMapStyle NOTIFY currentMapStyleChanged FINAL)
    Q_PROPERTY(int currentMapIndex READ currentMapIndex WRITE setCurrentMapIndex NOTIFY currentMapIndexChanged FINAL)
    Q_PROPERTY(QUrl currentMapImgPath READ currentMapImgPath WRITE setCurrentMapImgPath NOTIFY currentMapImgPathChanged FINAL)
    Q_PROPERTY(QUrl particleFilePath READ particleFilePath WRITE setParticleFilePath NOTIFY particleFilePathChanged FINAL)
    Q_PROPERTY(QString primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged FINAL)
    Q_PROPERTY(QString secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged FINAL)
    Q_PROPERTY(QString highlightColor READ highlightColor WRITE setHighlightColor NOTIFY highlightColorChanged FINAL)
    Q_PROPERTY(QString particleColor READ particleColor WRITE setParticleColor NOTIFY particleColorChanged FINAL)
    Q_PROPERTY(int noteSpeed READ noteSpeed WRITE setNoteSpeed NOTIFY noteSpeedChanged FINAL)
    Q_PROPERTY(QString laneTheme READ laneTheme WRITE setLaneTheme NOTIFY laneThemeChanged FINAL)
    Q_PROPERTY(bool gameStart READ gameStart WRITE setGameStart NOTIFY gameStartChanged FINAL)
    Q_PROPERTY(bool titleScreen READ titleScreen WRITE setTitleScreen NOTIFY titleScreenChanged FINAL)
    Q_PROPERTY(int mapCount READ mapCount WRITE setMapCount NOTIFY mapCountChanged FINAL)
    Q_PROPERTY(QString currentMapIllustration READ currentMapIllustration WRITE setCurrentMapIllustration NOTIFY currentMapIllustrationChanged FINAL)

public:
    explicit DataHandlerModel(QObject* parent = nullptr, MessageHandler* msgHandler = nullptr);

    QList <QSharedPointer<MapData>> objects;
    AudioHandler* audioHandler;
    QSharedPointer<MapData>currentMapObject;
    MessageHandler* messageHandler;

    QString audioFilePath = "";

    void readMapFiles();


    QString currentMapTitle() const;
    void setCurrentMapTitle(const QString &newCurrentMapTitle);

    QString currentMapArtist() const;
    void setCurrentMapArtist(const QString &newCurrentMapArtist);

    int currentMapBPM() const;
    void setCurrentMapBPM(int newCurrentMapBPM);

    int currentMapDifficulty() const;
    void setCurrentMapDifficulty(int newCurrentMapDifficulty);

    QString currentMapStyle() const;
    void setCurrentMapStyle(const QString &newCurrentMapStyle);

    int currentMapIndex() const;
    void setCurrentMapIndex(int newCurrentMapIndex);

    QUrl currentMapImgPath() const;
    void setCurrentMapImgPath(const QUrl &newCurrentMapImgPath);

    QString primaryColor() const;
    void setPrimaryColor(const QString &newPrimaryColor);

    QString secondaryColor() const;
    void setSecondaryColor(const QString &newSecondaryColor);

    QString highlightColor() const;
    void setHighlightColor(const QString &newHighlightColor);

    QString particleColor() const;
    void setParticleColor(const QString &newParticleColor);

    QUrl particleFilePath() const;
    void setParticleFilePath(const QUrl &newParticleFilePath);

    int noteSpeed() const;
    void setNoteSpeed(int newNoteSpeed);

    QString laneTheme() const;
    void setLaneTheme(const QString &newLaneTheme);

    bool gameStart() const;
    void setGameStart(bool newGameStart);

    bool titleScreen() const;
    void setTitleScreen(bool newTitleScreen);

    int mapCount() const;
    void setMapCount(int newMapCount);

    QString currentMapIllustration() const;
    void setCurrentMapIllustration(const QString &newCurrentMapIllustration);

public slots:
    void mapRefresh();
    void setCurrentMapDetails();
    void prepMapSelect();
    void uninitializeMusic();
    void attemptRandomTrackStart(int num);
signals:
    void titleTrackStarted(QString title);
    void dataRefreshed();
    void currentMapTitleChanged();

    void currentMapArtistChanged();

    void errorOccurred(QString text);

    void currentMapBPMChanged();

    void currentMapDifficultyChanged();

    void currentMapStyleChanged();

    void currentMapIndexChanged();

    void currentMapImgPathChanged();

    void primaryColorChanged();

    void secondaryColorChanged();

    void highlightColorChanged();

    void particleColorChanged();

    void particleFilePathChanged();


    void noteSpeedChanged();

    void laneThemeChanged();

    void gameStartChanged();

    void titleScreenChanged();

    void mapCountChanged();

    void currentMapIllustrationChanged();

private:

    enum Roles{
        Title = Qt::UserRole, //Use Qt::UserRole on first element to avoid conflicts with default Qenums
        Artist,
        BPM,
        Difficulty,
        Style
    };

    //Required to work the model
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void playTrackPreview(const int startTime, const int endTime);
    bool trackPreviewActivated = false;

    int m_mapCount = 0;
    int m_noteSpeed = 0;
    int m_currentMapBPM = 0;
    int m_currentMapDifficulty = 0;
    int m_currentMapIndex = 0;
    QUrl m_currentMapImgPath;
    QUrl m_particleFilePath;
    QString m_currentMapTitle = "";
    QString m_currentMapArtist = "";
    QString m_currentMapStyle = "";
    QString m_primaryColor = "";
    QString m_secondaryColor = "";
    QString m_highlightColor = "";
    QString m_particleColor = "";
    QString m_laneTheme = "rebirth";
    bool m_gameStart = false;
    bool m_titleScreen = true;
    QString m_currentMapIllustration;
};

#endif // DATAHANDLERMODEL_H
