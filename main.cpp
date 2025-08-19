#define MINIAUDIO_IMPLEMENTATION
#include "DataHandlerModel.h"
#include "GameInitializer.h"
#include "MessageHandler.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "NotePool.h"



int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    app.setWindowIcon(QIcon(":/RebirthIcon.ico"));

    MessageHandler* msgHandler = new MessageHandler(&app);
    DataHandlerModel* dataHandler = new DataHandlerModel(&app, msgHandler);
    GameInitializer *init = new GameInitializer(&app, &engine, dataHandler, msgHandler);
    init->setup(); //Call this first thing so QML can recognize gameManager
    qmlRegisterUncreatableType<MessageHandler>("MessageHandler", 1, 0, "MessageHandlerEnums", "messageHandler statuses");
    engine.rootContext()->setContextProperty("messageHandler", msgHandler);
    engine.rootContext()->setContextProperty("dataHandlerModel", dataHandler);
    engine.rootContext()->setContextProperty("gameInitializer", init);

    //Initializing engine here instead of below will crash the app

    QObject::connect( //Loading QML elements
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Rebirth", "Main");


    /* Instant crash occurs when we call ma_engine_init before engine creation,
      so we're calling it here */

    init->audioHandler->initializeEngine();
    if(init->audioHandler->engineResult != MA_SUCCESS){
        QCoreApplication::exit(404);
    }

    //Get Window (Gameplay mechanics rely on this)
    if (!engine.rootObjects().isEmpty()){
        const QList<QObject*> roots = engine.rootObjects();
        QObject* root = roots.first();
        if (root){
            QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
            if (window){
                window->setWindowState(Qt::WindowMaximized);

                window->setFlags(Qt::FramelessWindowHint);
                init->window = window; //Window ref should be obtained after qml elements are loaded
                init->notePool->windowObj = window;
            }
            else{
               QCoreApplication::exit(404);
            }

        }
        else{
           QCoreApplication::exit(404);
        }
    }

    /* Debug functions: call when switching to game scene
    Be sure to simultaneously hard code a currentMapIndex() value in DataHandler and switch to the scene in Main.qml*/
    // dataHandler->setGameStart(true);
    // dataHandler->setTitleScreen(false);
    // dataHandler->mapRefresh();

    return app.exec();
}
