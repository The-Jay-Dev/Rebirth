#include "GameInitializer.h"
#include "GameManager.h"
#include "HitObjectEnums.h"
#include "HitObjectModel.h"
#include "miniaudio.h"
#include "NotePool.h"
#include "TimeStep.h"
#include <QDir>
#include <QQmlContext>
#include <QCoreApplication>

void GameInitializer::tick(int deltatime)
{
    conductor->tick(deltaTimer->deltatime);
    gameManager->tick(deltaTimer->deltatime);

    emit threadTick(deltatime);
}

GameInitializer::~GameInitializer()
{
    notePoolThread->quit();
    notePoolThread->wait();
    delete notePool;
    delete notePoolThread;
    deltaTimer->quit();
    deltaTimer->wait();
    delete deltaTimer;
}

void GameInitializer::setup(){

    audioHandler = new AudioHandler(this, messageHandler);

    notePoolThread = new QThread;

    //Conductor calculates beats, measures, and song position
    conductor = new MusicConductor(this, audioHandler);

    deltaTimer = new TimeStep(this);

    judgementLine = new JudgementLineModel(this);

    dataHandlerModel->audioHandler = audioHandler;

    notePool = new NotePool(conductor,judgementLine, audioHandler);
    notePool->moveToThread(notePoolThread);

    gameManager = new GameManager(this, messageHandler, audioHandler, dataHandlerModel,conductor,notePool, this);
    hitModelObj = new HitObjectModel(this, messageHandler, notePool);  //Falling note handler
    notePool->hitObjectModel = hitModelObj;
    notePool->gameManager = gameManager;
    appEngine->rootContext()->setContextProperty("audioHandler", audioHandler);
    appEngine->rootContext()->setContextProperty("gameManager", gameManager);
    appEngine->rootContext()->setContextProperty("hitObjectModel", hitModelObj);
    appEngine->rootContext()->setContextProperty("judgementLineHandler", judgementLine);
    qmlRegisterUncreatableType<HitObjectEnums>("HitObjectEnums", 1, 0, "HitObjectEnums", "HitObject characteristics");
    setAppDirectory(QCoreApplication::applicationDirPath());

    connect(notePoolThread, &QThread::started, notePool, &NotePool::gameStart, Qt::QueuedConnection);
    connect(notePool, &NotePool::startGame, this, &GameInitializer::gameStart, Qt::QueuedConnection);
}

void GameInitializer::begin()
{

    if (hitModelObj && notePool && deltaTimer && conductor && dataHandlerModel && gameManager && !dataHandlerModel->objects.isEmpty()){

        notePoolThread->quit();
        notePoolThread->wait();

        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
        dataHandlerModel->setGameStart(true);

        if (audioHandler->validSoundObject)
            audioHandler->uninitializeMusic();

        audioHandler->reInstallEngine();
        audioHandler->initializeSoundEffects();
        audioHandler->initializeMusic(dataHandlerModel->audioFilePath);

        conductor->dataHandler = dataHandlerModel;
        notePoolThread->start(QThread::NormalPriority);

    }

}

void GameInitializer::gameStart()
{

    conductor->gameStart(); //Should be called before playFromBeat()

    /* Call for debugging/mapping */
     //conductor->playFromBeat(0);

     //All gameManager calls must come after conductor calls
    gameManager->stateList = dataHandlerModel->currentMapObject->stateList;
    gameManager->gameStart();

    if(window)
        gameManager->getWindow(window);


    connect(this, &GameInitializer::threadTick, notePool, &NotePool::tick, Qt::QueuedConnection);
    deltaTimer->start(QThread::HighPriority);
}

void GameInitializer::restoreCursor() const
{
    QGuiApplication::restoreOverrideCursor();
}

QString GameInitializer::appDirectory() const
{
    return m_appDirectory;
}

void GameInitializer::setAppDirectory(const QString &newAppDirectory)
{
    if (m_appDirectory == newAppDirectory)
        return;
    m_appDirectory = newAppDirectory;
    emit appDirectoryChanged();
}
