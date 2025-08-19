#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "AudioHandler.h"
#include "MessageHandler.h"
#include "MusicConductor.h"
#include "NotePool.h"
#include "EmotionalCapacityStates.h"
#include <QObject>
#include <QQuickWindow>
class GameInitializer;

class GameManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int combo READ combo WRITE setCombo NOTIFY comboChanged FINAL)
    Q_PROPERTY(int score READ score  WRITE setScore  NOTIFY scoreChanged FINAL)
    Q_PROPERTY(int capacityProgress READ capacityProgress WRITE setCapacityProgress NOTIFY capacityProgressChanged FINAL)
    Q_PROPERTY(Rank rank READ rank WRITE setRank NOTIFY rankChanged FINAL)
    Q_PROPERTY(int perfectCount READ perfectCount WRITE setPerfectCount NOTIFY perfectCountChanged FINAL)
    Q_PROPERTY(int goodCount READ goodCount WRITE setGoodCount NOTIFY goodCountChanged FINAL)
    Q_PROPERTY(int missCount READ missCount WRITE setMissCount NOTIFY missCountChanged FINAL)
    Q_PROPERTY(int noteCount READ noteCount WRITE setNoteCount NOTIFY noteCountChanged FINAL)
    Q_PROPERTY(int forgottenNoteCount READ forgottenNoteCount WRITE setforgottenNoteCount NOTIFY forgottenNoteCountChanged FINAL)
    Q_PROPERTY(bool highScoreAchieved READ highScoreAchieved WRITE setHighScoreAchieved NOTIFY highScoreAchievedChanged FINAL)
    Q_PROPERTY(int highScore READ highScore WRITE setHighScore NOTIFY highScoreChanged FINAL)
    Q_PROPERTY(Memory memoryStatus READ memoryStatus WRITE setMemoryStatus NOTIFY memoryStatusChanged FINAL)

public:


    GameManager(QObject* parent = nullptr, MessageHandler* msgHandler = nullptr,
                AudioHandler* audioHandlerObj = nullptr, DataHandlerModel* config = nullptr,
                MusicConductor* musicConductor= nullptr, NotePool* pool = nullptr, GameInitializer* init = nullptr)
        : QObject(parent), audioHandler(audioHandlerObj), dataConfig(config), conductor(musicConductor),
        notePool(pool),initializer(init){};

    QVector<EmotionalCapacityStates> stateList;

    void gameStart();
    void tick(float deltatime);
    void getWindow(QQuickWindow* window);
    void windowHeightChanged(int height);
    void setScore(int newScore);
    void setCombo(int newCombo);
    bool eventFilter(QObject* obj, QEvent* event) override;


    int combo() const;
    int score() const;

    enum Memory{
        Preserved,
        Recovered,
        Forgotten
    };
    Q_ENUM(Memory);


    enum Rank{
        S,
        A,
        B,
        C,
        D
    };
    Q_ENUM(Rank);

    int capacityProgress() const;
    void setCapacityProgress(int newCapacityProgress);

    Rank rank() const;
    void setRank(Rank newRank);

    int perfectCount() const;
    void setPerfectCount(int newPerfectCount);

    int goodCount() const;
    void setGoodCount(int newGoodCount);

    int missCount() const;
    void setMissCount(int newMissCount);

    int noteCount() const;
    void setNoteCount(int newNoteCount);

    bool highScoreAchieved() const;
    void setHighScoreAchieved(bool newHighScoreAchieved);

    int highScore() const;
    void setHighScore(int newHighScore);

    Memory memoryStatus() const;
    void setMemoryStatus(const Memory &newMemoryStatus);

    int forgottenNoteCount() const;
    void setforgottenNoteCount(int newforgottenNoteCount);

signals:
    void createHitObject(int beat, int lane, QString type);

    void lanePressed(int lane);

    void laneHeld(int lane);

    void windowHeightAdjusted(float height);

    void laneReleased(int lane);

    void comboChanged();

    void resultCalculated(Memory status);

    void levelComplete();

    void scoreChanged();

    void capacityProgressChanged(int progress, QString stateName, int capacityNumber);

    void rankChanged();

    void perfectCountChanged();

    void goodCountChanged();

    void missCountChanged();

    void noteCountChanged();

    void highScoreAchievedChanged();

    void highScoreChanged();

    void memoryStatusChanged();

    void forgottenNoteCountChanged();

    void gameEnd();
public slots:
    void InitializeNote(int index);
    void noteHit(QString judgement);
    void noteMiss(QString judgement);
    void calculateResult();
private:


    DataHandlerModel *dataConfig;
    MusicConductor * conductor;
    GameInitializer *initializer;
    NotePool * notePool;
    MessageHandler* messageHandler;

    QString currentStateTitle = "";


    AudioHandler* audioHandler;

    int noteIndex = 0;
    int songPos = 0;
    int capacityNum = 0;
    int totalNotes = 0;
    int totalForgottenNotes = 0;
    int hitScore = 200;
    int maxScore = 1000000;
    int comboMultiplier = 1;
    int preparationBeats = 1;

    float noteSpawnBeat = 0.0;
    float difficultyMultiplier = 1;

    bool endOfNotes = false;
    bool lanePressedStatuses[4] {false,false,false,false};

    Rank m_rank = D;
    Memory m_memoryStatus = Forgotten;

    void resetCounters();
    int calculateTotalNotes();
    void calculateProgress();
    void endGame();
    inline float calculateNoteSpawnTime();

    void increaseJudgementCount(QString judgement);

    int m_combo = 0;
    int m_score = 0;
    int basePoints = 0;
    int m_noteCount = 0;
    int m_goodCount = 0;
    int m_missCount = 0;
    int m_perfectCount = 0;
    int m_capacityProgress = 0;
    int m_highScore = 0;
    int m_forgottenNoteCount = 0;
    float totalOffset = 0.0;
    bool m_highScoreAchieved = false;

};

#endif // GAMEMANAGER_H
