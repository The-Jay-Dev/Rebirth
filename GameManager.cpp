#include "GameManager.h"
#include "miniaudio.h"
#include "GameInitializer.h"
#include "TimeStep.h"


void GameManager::gameStart()
{
    endOfNotes = false;
    noteIndex = 0; //Zero-based

    resetCounters();
    capacityNum = stateList.length();
    preparationBeats = dataConfig->noteSpeed();
    totalOffset = conductor->totalOffsetBeats;

    if(!stateList.isEmpty() && 0 < stateList.size()){

        currentStateTitle =  stateList[0].state;
        setCapacityProgress(0);

        //Ran automatically when setProgress is called, but we're at the game-start, so we have to call it manually here as well
        emit capacityProgressChanged(capacityProgress(), currentStateTitle, capacityNum);

        basePoints = maxScore/calculateTotalNotes();

        if (conductor->playFromBeatActive){
            calculateProgress();
        }
    }
    else{
       emit messageHandler->logMessage("Map data file invalid:"
                                        "\nEmotional Capacity object may be empty."
                                        "\nPlease check that file uses the rgmap extension and follows the map file guidelines"
                                        "Abort.", MessageHandler::FAILED);

    }
    connect(notePool, &NotePool::noteHit, this, &GameManager::noteHit);
    connect(notePool, &NotePool::noteMiss, this, &GameManager::noteMiss);
    connect(notePool, &NotePool::noteHit, this, &GameManager::increaseJudgementCount);

}

void GameManager::tick(float deltatime)
{
    if (conductor && !endOfNotes && !stateList.isEmpty() && capacityProgress() < stateList.size()){
        float noteListLength = stateList[capacityProgress()].notes.length();

        if (noteIndex < noteListLength){

            if (conductor->currentTime >= calculateNoteSpawnTime()){ //Spawn notes "x" beats before hit time
                InitializeNote(noteIndex);
                noteIndex++;
            }
        }
        else if (noteIndex >= (noteListLength) && (capacityProgress() + 1) < capacityNum){
            currentStateTitle = stateList[capacityProgress() + 1].state; //Always update title before progress due to signal
            setCapacityProgress(capacityProgress() + 1);
            noteIndex = 0;
        }
        else if ( capacityProgress() + 1 >= capacityNum && noteIndex >= stateList[capacityProgress()].notes.length())
            endOfNotes = true;
    }

    if(conductor && audioHandler->validSoundObject && conductor->songStarted && !ma_sound_is_playing(&audioHandler->sound)){
        endGame();
    }

}


void GameManager::endGame()
{
    initializer->deltaTimer->quit();
    conductor->songStarted = false;

    emit gameEnd();
    notePool->clearPool();

    delete conductor->elapsedTimer;
    conductor->elapsedTimer = nullptr;

    for (int i = 0; i < notePool->maSoundList.length(); i++){
        ma_sound * obj = notePool->maSoundList.at(i);
        ma_sound_uninit(obj);
        notePool->maSoundList.removeAt(i);
        delete obj;
    }
    disconnect(notePool, &NotePool::noteHit, this, &GameManager::noteHit);
    disconnect(notePool, &NotePool::noteMiss, this, &GameManager::noteMiss);
    disconnect(notePool, &NotePool::noteHit, this, &GameManager::increaseJudgementCount);
    emit levelComplete();
}

float GameManager::calculateNoteSpawnTime()
{
    float noteBeat = stateList[capacityProgress()].notes[noteIndex].beat + conductor->totalOffsetBeats;
    noteSpawnBeat = noteBeat - preparationBeats;
    return noteSpawnBeat * conductor->secsPerBeat;
}

void GameManager::increaseJudgementCount(QString judgement)
{
    if(judgement == "Perfect"){
        setPerfectCount(perfectCount() + 1);
        setNoteCount(noteCount() + 1);
        return;
    }
    else if (judgement == "Good"){
        setGoodCount(goodCount() + 1);
        setNoteCount(noteCount() + 1);
        return;
    }
    else if (judgement == "Forgotten"){
        setMissCount(missCount() + 1);
        return;
    }

    if(judgement != "Miss"){
        setNoteCount(noteCount() + 1);
    }
}

void GameManager::resetCounters()
{
    totalNotes = 0;
    totalForgottenNotes = 0;
    setPerfectCount(0);
    setGoodCount(0);
    setMissCount(0);
    setforgottenNoteCount(0);
    setNoteCount(0);
    setCombo(0);
    setScore(0);
}

int GameManager::calculateTotalNotes()
{
    for(int i = 0; i< stateList.length(); i++){
        for(int j = 0; j< stateList[i].notes.length(); j++){
            if(stateList[i].notes[j].type == HitObjectEnums::Forgotten)
                totalForgottenNotes++;
            totalNotes++;
        }
    }
    return totalNotes;
}

void GameManager::calculateProgress() //Called when playFromBeat is used (Conductor)
{
    for(int i = 0; i< stateList.length(); i++){
        for(int j = 0; j< stateList[i].notes.length(); j++){
            noteSpawnBeat = stateList[i].notes[j].beat - preparationBeats;
            if(noteSpawnBeat > conductor->beatsSkipped){
                setCapacityProgress(i);
                noteIndex = j;
                return;
            }
        }
    }
}

void GameManager::windowHeightChanged(int height)
{
    emit windowHeightAdjusted(height);
}



bool GameManager::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress){
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        int key = keyEvent->key();

        if (!keyEvent->isAutoRepeat()){

            switch (key) {
            case Qt::Key_A:

                if(!lanePressedStatuses[0]){
                    lanePressedStatuses[0] = true;
                    emit lanePressed(0);
                }
                emit laneHeld(0);

                break;
            case Qt::Key_S:
                if( !lanePressedStatuses[1]){
                    lanePressedStatuses[1] = true;
                    emit lanePressed(1);

                }

                emit laneHeld(1);


                break;
            case Qt::Key_D:
                if(!lanePressedStatuses[2]){
                    lanePressedStatuses[2] = true;
                    emit lanePressed(2);

                }

                emit laneHeld(2);


                break;
            case Qt::Key_F:
                if(!lanePressedStatuses[3]){
                    lanePressedStatuses[3] = true;
                    emit lanePressed(3);

                }

                emit laneHeld(3);


                break;
            default:

                break;
            }
            return true;

        }
    }

    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (!keyEvent->isAutoRepeat()){
            switch (keyEvent->key()) {
            case Qt::Key_A:

                lanePressedStatuses[0] = false;
                emit laneReleased(0);
                break;
            case Qt::Key_S:
                lanePressedStatuses[1] = false;
                emit laneReleased(1);
                break;
            case Qt::Key_D:
                lanePressedStatuses[2] = false;
                emit laneReleased(2);
                break;
            case Qt::Key_F:
                lanePressedStatuses[3] = false;
                emit laneReleased(3);
                break;
            default:
                break;
            }

            return true;
        }

    }


    return QObject::eventFilter(obj,event);

}

void GameManager::getWindow(QQuickWindow* window)
{

    if (window){
        window->installEventFilter(this);
        connect(window, &QQuickWindow::heightChanged, this, &GameManager::windowHeightChanged);
        windowHeightChanged(window->height());
        return;
    }


}

void GameManager::InitializeNote(int index)
{


    float noteBeat = stateList[capacityProgress()].notes[index].beat + totalOffset;
    if (notePool){
        if (stateList[capacityProgress()].notes[index].type == HitObjectEnums::Tap ){

            notePool->createNote(noteBeat, stateList[capacityProgress()].notes[index].lane, stateList[capacityProgress()].notes[index].type, preparationBeats);
        }
        else if(stateList[capacityProgress()].notes[index].type ==  HitObjectEnums::DoubleTap){

            notePool->createNote(noteBeat, stateList[capacityProgress()].notes[index].lane, stateList[capacityProgress()].notes[index].type, preparationBeats, stateList[capacityProgress()].notes[index].duration);
        }
        else{ //hold note
            notePool->createNote(noteBeat, stateList[capacityProgress()].notes[index].lane, stateList[capacityProgress()].notes[index].type, preparationBeats, stateList[capacityProgress()].notes[index].duration, stateList[capacityProgress()].notes[index].tailLane);
        }
    }

}

void GameManager::noteHit(QString judgement)
{
    if(judgement == "Perfect"){
        hitScore = 1 * basePoints;
    }
    else if (judgement == "Good")
    {
        hitScore = 0.5 * basePoints;

    }
    else if (judgement == "Forgotten")
    {
        hitScore = -basePoints;
        setCombo(0);
        comboMultiplier = 1;
    }


    if (judgement != "Forgotten")
    {
        setScore(qMax(qFloor(score() + (hitScore * comboMultiplier)),0));
        setCombo(combo() + 1);
        comboMultiplier ++;
    }
    else
    {
         setScore(qMax(qFloor(score() + hitScore),0));
    }
}

void GameManager::noteMiss(QString judgement)
{

    setMissCount(missCount() + 1);
    setCombo(0);
    comboMultiplier = 1;
}

void GameManager::calculateResult()
{
    float weightedHits = (perfectCount() * 1) + (goodCount() * 0.7);

    float accuracy = (noteCount() > 0) ?  std::clamp((weightedHits / (totalNotes - totalForgottenNotes)) * 100.0f, 0.0f, 100.0f) : 0;
    if (accuracy > 94.99){
        setRank(S);
        setMemoryStatus(Preserved);
    }
    else if (accuracy >= 89.99){
        setRank(A);
        setMemoryStatus(Preserved);
    }
    else if (accuracy >= 79.99){
        setRank(B);
        setMemoryStatus(Recovered);
    }
    else if (accuracy >= 59.99){
        setRank(C);
        setMemoryStatus(Recovered);
    }

    else {
        setRank(D);
        setMemoryStatus(Forgotten);
    }

    emit resultCalculated(memoryStatus());
}

int GameManager::combo() const
{
    return m_combo;
}

void GameManager::setCombo(int newCombo)
{
    if (m_combo == newCombo)
        return;
    m_combo = newCombo;
    emit comboChanged();
}

int GameManager::score() const
{
    return m_score;
}

void GameManager::setScore(int newScore)
{
    if (m_score == newScore)
        return;
    m_score = newScore;
    emit scoreChanged();
}

int GameManager::capacityProgress() const
{
    return m_capacityProgress;
}

void GameManager::setCapacityProgress(int newCapacityProgress)
{
    if (m_capacityProgress == newCapacityProgress)
        return;
    m_capacityProgress = newCapacityProgress;
    emit capacityProgressChanged(m_capacityProgress, currentStateTitle, capacityNum);

}

GameManager::Rank GameManager::rank() const
{
    return m_rank;
}

void GameManager::setRank(Rank newRank)
{
    if (m_rank == newRank)
        return;
    m_rank = newRank;
    emit rankChanged();
}

int GameManager::perfectCount() const
{
    return m_perfectCount;
}

void GameManager::setPerfectCount(int newPerfectCount)
{
    if (m_perfectCount == newPerfectCount)
        return;
    m_perfectCount = newPerfectCount;
    emit perfectCountChanged();
}

int GameManager::goodCount() const
{
    return m_goodCount;
}

void GameManager::setGoodCount(int newGoodCount)
{
    if (m_goodCount == newGoodCount)
        return;
    m_goodCount = newGoodCount;
    emit goodCountChanged();
}

int GameManager::missCount() const
{
    return m_missCount;
}

void GameManager::setMissCount(int newMissCount)
{
    if (m_missCount == newMissCount)
        return;
    m_missCount = newMissCount;
    emit missCountChanged();
}

int GameManager::noteCount() const
{
    return m_noteCount;
}

void GameManager::setNoteCount(int newNoteCount)
{
    if (m_noteCount == newNoteCount)
        return;
    m_noteCount = newNoteCount;
    emit noteCountChanged();
}

bool GameManager::highScoreAchieved() const
{
    return m_highScoreAchieved;
}

void GameManager::setHighScoreAchieved(bool newHighScoreAchieved)
{
    if (m_highScoreAchieved == newHighScoreAchieved)
        return;
    m_highScoreAchieved = newHighScoreAchieved;
    emit highScoreAchievedChanged();
}

int GameManager::highScore() const
{
    return m_highScore;
}

void GameManager::setHighScore(int newHighScore)
{
    if (m_highScore == newHighScore)
        return;
    m_highScore = newHighScore;
    emit highScoreChanged();
}

GameManager::Memory GameManager::memoryStatus() const
{
    return m_memoryStatus;
}

void GameManager::setMemoryStatus(const Memory &newMemoryStatus)
{
    if (m_memoryStatus == newMemoryStatus)
        return;
    m_memoryStatus = newMemoryStatus;
    emit memoryStatusChanged();
}

int GameManager::forgottenNoteCount() const
{
    return m_forgottenNoteCount;
}

void GameManager::setforgottenNoteCount(int newforgottenNoteCount)
{
    if (m_forgottenNoteCount == newforgottenNoteCount)
        return;
    m_forgottenNoteCount = newforgottenNoteCount;
    emit forgottenNoteCountChanged();
}
