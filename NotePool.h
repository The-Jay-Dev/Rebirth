#ifndef NOTEPOOL_H
#define NOTEPOOL_H

#include "HitObject.h"
#include "HitObjectEnums.h"
#include "JudgementLineModel.h"
#include "MusicConductor.h"
#include "HitObjectEnums.h"
#include <QAbstractListModel>
#include <QRunnable>
#include <QObject>
#include <QQuickWindow>
#include <QRect>
#include <QMutex>

class HitObjectModel;
class GameManager;

class NotePool : public QObject
{
    Q_OBJECT



    Q_PROPERTY(QString noteType READ noteType WRITE setNoteType NOTIFY noteTypeChanged FINAL)


public:
    explicit NotePool(MusicConductor* musicConductor = nullptr,
                      JudgementLineModel* judgementLineModel = nullptr, AudioHandler* audioHandlerObj = nullptr);

    ~NotePool();

    void gameStart();

    //Indexes correspond to respective lanes

    QQuickWindow* windowObj;
    QVector<ma_sound*> maSoundList;
    float windowHeight;
    GameManager* gameManager;
    HitObjectModel* hitObjectModel;

    QString noteType() const;
    void setNoteType(const QString &newNoteType);

    void clearPool();

public slots:
    void createNote(float beat, int lane, HitObjectEnums::Types type, float fallduration, float duration = 5, int tailLane = 0);
    void holdFailed( int objIndex = 0, QSharedPointer<HitObject> obj = nullptr);
    void tick(float deltatime);
    void handleLanePressed(int lane);
    void handleLaneReleased(int lane);
    void setWindowHeight(float height);


signals:

    void addToObjectPool(QSharedPointer<HitObject> object);
    void errorOccurred(QString errorMessage);
    void spawnHoldEmitter(int lane);
    void despawnHoldEmitter(int lane);
    void spawnTrail(int index);
    void despawnTrail(int index);
    void spawnSecondTapJudgementRing(float scale);
    void updateSecondTapRingScale(float scale, float progress);
    void noteTypeChanged();
    void noteMiss(QString noteJudgement, int lane);
    void noteHit(QString noteJudgement, HitObjectEnums::ColorTypes colorType, float xPosition, float objectWidth );
    void colorChanged();
    void noteActiveChanged(int index, bool active);
    void spawnedGimmickObjChanged(int index);
    void onDespawnHoldNoteController();
    void noteVisibilityChanged(int index, bool visible);
    void setRelativeMouseX(float x);
    void appendTapNote(QSharedPointer<HitObject>);
    void updateQml(QSharedPointer<HitObject> object);
    void alert(QString msg, int messageStatus = 2);
    void updateRelativeMouseX(float x);
    void clearModel();
    void startGame();

private:


    enum Roles{
        XRole = Qt::UserRole, //Use Qt::UserRole on first element to avoid conflicts with default Qenums
        YRole,
        Lane,
        Beat,
        Type,
        Active,
        ColorType,
        TailNote,
        HoldFailed,
        JudgementRingMaxScale,
        DebugText,
        TailNoteXRole,
        TailNoteYRole,
        Visible,
        SlideEventReady
    };


    QVector<QSharedPointer<HitObject>> tapNotes;
    QVector<QSharedPointer<HitObject>> holdNotes;
    QVector<QSharedPointer<HitObject>> doubleTapNotes;
    QVector<QSharedPointer<HitObject>> forgottenNotes;
    QVector<QSharedPointer<HitObject>> slideNotes;
    QVector<QSharedPointer<HitObject>> updatedNotes;


    const int OBJECT_POOL_SIZE = 80;
    const int OBJECT_POOL_MINIMUM = 20;

    QString noteJudgement = "";

    float speed = 0.0;
    float beatOffset = 0.0;
    float validMs  = 0.0;
    float centerOffset = 0.0;

    int screenTopOffset = 100;
    int boundaryOffset = 50;

    QRect screenGeometry;
    MusicConductor* conductor;
    JudgementLineModel* judgementLine;
    AudioHandler* audioHandler;

    QString judgement( QSharedPointer<HitObject> object,float distance, int index,  bool destroyNote = false);
    float getScreenTop();
    float getScreenBottom();


    HitObjectEnums::ColorTypes determineColor(HitObjectEnums::Types type, float beat);
    bool holdNoteActivationCheck(QSharedPointer<HitObject> obj,QSharedPointer<HitObject> tailObject, float yPos, int lane, float distance);

    float getLaneXPosition(int lane);
    void calculateSlideNoteHeadXPosition( QSharedPointer<HitObject> object);
    void holdActivated( QSharedPointer<HitObject> object);
    void holdActive( QSharedPointer<HitObject> object);
    void noteMissed( QSharedPointer<HitObject> object);
    void holdNoteHeadObject( QSharedPointer<HitObject> object, int index, float validMs, float beatOffset);
    void doubleTapNoteObject( QSharedPointer<HitObject> object, int noteIndex, float validMs, float beatOffset);
    void holdNoteTailObject( QSharedPointer<HitObject> object, float index, float spawnY, QVector<QSharedPointer<HitObject>> holdNotes);
    void noteYPosition( QSharedPointer<HitObject> object, float validMs, float SpawnY);
    void atScreenBottomCheck( QSharedPointer<HitObject> object, int index);
    void updateObjectPoolIndex( QSharedPointer<HitObject> object);
    void tailNoteMovement( QSharedPointer<HitObject> object, float spawnY);
    void resetNote(QSharedPointer<HitObject>object);


    void playHitSoundA();
    void playHitSoundB();
    void playHitSoundC();
    void playHitSoundD();
    void updateAccuracyMilliseconds();
    inline void deleteHitSoundRefs();
    float centerMouse();

    int trackColumns = 4;
    int objectPoolIndex = 0;
    int tapNotePoolIndex = 0;
    int holdNotePoolIndex = 0;
    int doubleTapNotePoolIndex = 0;
    int forgottenNotePoolIndex = 0;


    float mostrecenetbeattime = 0;
    float nextBeattime = 3;
    double beatsUntilHit = 1.0;
    float missMs = 400;
    float greatMs = 150;
    float freeFallTime = .5;
    float belowLineMissMs = 0.0;
    float laneCenter = 0.0;

    float sfxVolume = 0.4;
    float slideValue = 0.0;
    float mouseStartX = 0.0;
    float relativeMouseX = 0.0;
    float laneUnit = 0.0;
    float noteWidth = 0.0;
    float noteHeight = 0.0;

    bool debug = false;


    QMutex mutex;

    QString m_noteType;

    bool laneHeldStatuses[4] {false,false,false,false};
    bool lanePressedStatuses[4] {false,false,false,false}; //Qt doesn't a singular signal for button presses, so we handle it ourselves for accuracy
    bool laneJudgementActiveStatuses[4] {false,false,false,false}; //Prevents simultaneous judgement calls
    bool gameEnd = false;
};



#endif // NOTEPOOL_H
