#ifndef GAMEINITIALIZER_H
#define GAMEINITIALIZER_H
#include "AudioHandler.h"
#include "DataHandlerModel.h"
#include "JudgementLineModel.h"
#include "MusicConductor.h"
#include "MessageHandler.h"
#include "miniaudio.h"
#include "MusicConductor.h"
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

class HitObjectModel;
class GameManager;
class TimeStep;
class NotePool;

class GameInitializer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appDirectory READ appDirectory WRITE setAppDirectory NOTIFY appDirectoryChanged FINAL)
public:

     GameInitializer(QObject *parent = nullptr, QQmlApplicationEngine* engine = nullptr, DataHandlerModel* dataModel = nullptr, MessageHandler* msgHandler = nullptr) : QObject(parent), messageHandler(msgHandler), appEngine(engine),dataHandlerModel(dataModel) {
     }
     ~GameInitializer();

    void setup();
    QQuickWindow* window = nullptr;
    MessageHandler* messageHandler = nullptr;
    AudioHandler *audioHandler = nullptr;
    DataHandlerModel* dataHandlerModel = nullptr;
    HitObjectModel *hitModelObj = nullptr;
    TimeStep *deltaTimer = nullptr;
    NotePool *notePool = nullptr;
    QThread* notePoolThread = nullptr;

    QString appDirectory() const;
    void setAppDirectory(const QString &newAppDirectory);
    void tick(int deltatime);

public slots:
    void begin();
    void gameStart();
    void restoreCursor() const;
signals:
    void appDirectoryChanged();
    void threadTick( float deltatime);


private:

    JudgementLineModel *judgementLine;
    GameManager* gameManager;
    MusicConductor* conductor;
    QQmlApplicationEngine *appEngine;

    QString m_appDirectory;
};

#endif // GAMEINITIALIZER_H
