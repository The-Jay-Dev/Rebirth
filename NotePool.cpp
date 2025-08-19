#include "NotePool.h"
#include "GameManager.h"
#include "HitObjectModel.h"
#include <QThread>

NotePool::NotePool( MusicConductor* musicConductor, JudgementLineModel* judgementLineModel, AudioHandler* audioHandlerObj)
    : conductor(musicConductor), judgementLine(judgementLineModel), audioHandler(audioHandlerObj)
{
    screenGeometry = QGuiApplication::primaryScreen()->geometry();
}

NotePool::~NotePool()
{
    windowObj = nullptr;
    conductor = nullptr;
    judgementLine = nullptr;
    audioHandler = nullptr;

}

void NotePool::gameStart()
{
    QThread::currentThread()->setObjectName("NotePool Thread");
    qDebug() << "start";
    clearPool();
    gameEnd = false;
    laneUnit = hitObjectModel->noteTrackWidth()/trackColumns;
    float width = laneUnit * 0.8;
    noteWidth = width; //divide by 2 for even, smaller proportions
    noteHeight = width/6; //divide by 2 for even, smaller proportions

    if(OBJECT_POOL_SIZE >= OBJECT_POOL_MINIMUM && OBJECT_POOL_SIZE % 2 == 0){

        float tapNoteCount = 0;
        float holdNoteCount = 0;
        float doubleTapNoteCount = 0;
        float slideNoteCount = 0;
        float forgottenNoteCount = 0;

        for (int i = 0; i < OBJECT_POOL_SIZE; ++i)
        {
            QSharedPointer<HitObject> note = QSharedPointer<HitObject>::create(this);
            resetNote(note);
            note->objectPoolIndex = i;
            emit addToObjectPool(note);

            if(tapNoteCount < OBJECT_POOL_SIZE/2){
                tapNotes.append(note);
                tapNoteCount++;
            }
            else if (doubleTapNoteCount < OBJECT_POOL_SIZE/20){
                doubleTapNotes.append(note);
                doubleTapNoteCount++;
            }
            else if (holdNoteCount < OBJECT_POOL_SIZE/5){
                holdNotes.append(note);
                holdNoteCount++;
            }
            else if (forgottenNoteCount < OBJECT_POOL_SIZE/4){
                forgottenNotes.append(note);
                forgottenNoteCount++;
            }

        }

        emit startGame();
        qRegisterMetaType<HitObjectEnums::ColorTypes>("HitObjectEnums::ColorTypes");
        connect(hitObjectModel, &HitObjectModel::holdFailed, this, &NotePool::holdFailed, Qt::QueuedConnection);
        connect(gameManager, &GameManager::lanePressed, this, &NotePool::handleLanePressed,  Qt::QueuedConnection);
        connect(gameManager, &GameManager::laneReleased, this, &NotePool::handleLaneReleased,  Qt::QueuedConnection);
        connect(gameManager, &GameManager::windowHeightAdjusted, this, &NotePool::setWindowHeight,  Qt::QueuedConnection);
    }
    else
        emit alert("Critical Error: \nObject pool must be an even number and greater than " + QString::number(OBJECT_POOL_MINIMUM - 1) + " for divisions to work properly.\nThe game will not start.", 2);


}

void NotePool::tick(const float deltatime)
{
    if (!gameEnd){
        float spawnY = getScreenTop() - screenTopOffset;
        centerOffset = noteHeight/2; //Height may change on different sizes

        /*Check forgotten notes first so they take precedence in note judgement
         * This means that the forgotten note objects should be mapped to come first when in
         * close proximity with another note.
         */


        for(int i = 0; i < forgottenNotes.size(); i++){
            if(forgottenNotes[i] && forgottenNotes[i]->noteActive){

                speed = (qAbs((judgementLine->yPosition() - spawnY) - centerOffset) /beatsUntilHit);
                beatOffset  = (qAbs((forgottenNotes[i]->yPosition - judgementLine->yPosition()) - centerOffset ) / speed);
                validMs = conductor ? beatOffset * (conductor->secsPerBeat* 1000) : 0;

                noteYPosition(forgottenNotes[i], validMs, spawnY);
                if( !forgottenNotes[i]->missed && lanePressedStatuses[forgottenNotes[i]->lane - 1]
                    && !laneJudgementActiveStatuses[forgottenNotes[i]->lane - 1]){
                    //Attempt score judgement!
                    judgement(forgottenNotes[i], beatOffset , i, true);
                }
                atScreenBottomCheck(forgottenNotes[i],i);
            }
        }

        for(int i = 0; i < tapNotes.size(); i++){
            if(tapNotes[i] && tapNotes[i]->noteActive){

                speed = (qAbs((judgementLine->yPosition() - spawnY) - centerOffset) /beatsUntilHit);
                beatOffset  = (qAbs((tapNotes[i]->yPosition - judgementLine->yPosition()) - centerOffset ) / speed);
                validMs = conductor ? beatOffset * (conductor->secsPerBeat* 1000) : 0;
                noteYPosition(tapNotes[i], validMs, spawnY);

                if(lanePressedStatuses[tapNotes[i]->lane - 1] && !laneJudgementActiveStatuses[tapNotes[i]->lane - 1] && !tapNotes[i]->missed){
                    //Attempt score judgement!
                    judgement(tapNotes[i], beatOffset, i,  true);
                }
                atScreenBottomCheck(tapNotes[i],i);
            }

        }

        for(int i = 0; i < doubleTapNotes.size(); i++){

            if(doubleTapNotes[i] && doubleTapNotes[i]->noteActive){

                speed = (qAbs((judgementLine->yPosition() - spawnY) - centerOffset) /beatsUntilHit);
                beatOffset  = (qAbs((doubleTapNotes[i]->yPosition - judgementLine->yPosition()) - centerOffset ) / speed);
                validMs = conductor ? beatOffset * (conductor->secsPerBeat* 1000) : 0;

                if( !doubleTapNotes[i]->secondTapActive){
                    noteYPosition(doubleTapNotes[i], validMs, spawnY);
                }
                else{
                    doubleTapNotes[i]->yPosition = judgementLine->yPosition() - centerOffset;
                }

                if(doubleTapNotes[i]->noteActive){
                    doubleTapNoteObject(doubleTapNotes[i], i, validMs, beatOffset);
                    atScreenBottomCheck(doubleTapNotes[i],i);

                }
            }
        }

        for(int i = 0; i < holdNotes.size(); i++){

            if(holdNotes[i] && holdNotes[i]->noteActive){

                speed = (qAbs((judgementLine->yPosition() - spawnY) - centerOffset) /beatsUntilHit);
                beatOffset  = (qAbs((holdNotes[i]->yPosition - judgementLine->yPosition()) - centerOffset ) / speed);
                validMs = conductor ? beatOffset * (conductor->secsPerBeat* 1000) : 0;

                if (!holdNotes[i]->tailNote)
                {
                    noteYPosition(holdNotes[i], validMs, spawnY);
                    holdNoteHeadObject(holdNotes[i], holdNotes[i]->objectPoolIndex , validMs, beatOffset);
                    emit updateQml(holdNotes[i]);
                }


                if (holdNotes[i]->tailNote){
                    holdNoteTailObject(holdNotes[i], holdNotes[i]->objectPoolIndex, spawnY, holdNotes);
                }


                atScreenBottomCheck(holdNotes[i],i);
                if ( holdNotes[i]->yPosition + centerOffset > windowHeight + boundaryOffset){
                    resetNote(holdNotes[i]);
                    return;
                }
            }
        }

        deleteHitSoundRefs();

        for (int i = 0; i < 4; i++) {
            lanePressedStatuses[i] = false;
            laneJudgementActiveStatuses[i] = false;
        }
    }
}

void NotePool::handleLanePressed(int lane)
{
    lanePressedStatuses[lane] = true;
    laneHeldStatuses[lane] = true;
}

void NotePool::handleLaneReleased(int lane)
{
    lanePressedStatuses[lane] = false;
    laneHeldStatuses[lane] = false;
}

void NotePool::setWindowHeight(float height)
{
    windowHeight = height;
}

void NotePool::createNote(float beat, int lane, HitObjectEnums::Types type,float fallduration, float duration, int tailLane)
{

    beatsUntilHit = fallduration;
    updateAccuracyMilliseconds();

    if (type == HitObjectEnums::Hold  || type == HitObjectEnums::Slide ){

        QSharedPointer<HitObject>tailObj =  holdNotes[holdNotePoolIndex];

        if (tailLane == -1)
        {
            tailObj->lane = lane;
            tailObj->xPosition = getLaneXPosition(lane);
        }
        else
        {
            tailObj->lane = tailLane;
            tailObj->xPosition = getLaneXPosition(tailLane);
        }

        tailObj->beat =  beat;
        tailObj->yPosition = getScreenTop() - screenTopOffset;
        tailObj->debugText = "";
        tailObj->type = HitObjectEnums::Hold;
        tailObj->noteHoldDuration = duration;
        tailObj->tailObject = nullptr;
        tailObj->missed =  false;
        tailObj->tailNote = true;
        tailObj->noteActive = true;
        tailObj->visible = true;
        tailObj->holdFailed = false;
        tailObj->holdActivated = false;
        tailObj->totalTravelDistance = 0;
        tailObj->slideEventReady = false;
        tailObj->colorType = determineColor(type, beat);

        updateObjectPoolIndex(tailObj);


        QSharedPointer<HitObject> headObj = holdNotes[holdNotePoolIndex];

        headObj->xPosition = getLaneXPosition(lane);
        headObj->yPosition = getScreenTop() - screenTopOffset;
        headObj->beat =  beat;
        headObj->lane = lane;
        headObj->type = type;
        headObj->noteHoldDuration = 0;
        headObj->tailObject = tailObj;
        headObj->missed = false;
        headObj->debugText = "";
        headObj->holdActivated = false;
        headObj->holdFailed = false;
        headObj->noteActive = true;
        headObj->visible = true;
        headObj->tailNote = false;
        headObj->totalTravelDistance = 0;
        headObj->laneXPos = headObj->xPosition;
        headObj->colorType = determineColor(type, beat);
        headObj->slideEventReady = false;
        headObj->tickSoundBeat = conductor->songPosInBeats;

        tailObj->headObject = headObj;
        updateObjectPoolIndex(headObj);
        emit spawnTrail( headObj->objectPoolIndex);


        emit updateQml(tailObj);
        emit updateQml(headObj);


    }
    else
    {

        QSharedPointer<HitObject>obj;

        switch(type){
        case HitObjectEnums::Tap:obj = tapNotes[tapNotePoolIndex];  break;
        case HitObjectEnums::DoubleTap:obj = doubleTapNotes[doubleTapNotePoolIndex];   break;
        case HitObjectEnums::Forgotten: obj = forgottenNotes[forgottenNotePoolIndex];  break;
        default:break;
        }

        obj->xPosition = getLaneXPosition(lane);
        obj->yPosition = getScreenTop() - screenTopOffset;
        obj->beat =  beat;
        obj->lane = lane;
        obj->type = type;
        obj->debugText = "";
        obj->noteHoldDuration = 0;
        obj->totalTravelDistance = 0;
        obj->missed = false;
        obj->tailObject = nullptr;
        obj->tailNote = false;
        obj->noteActive = true;
        obj->visible = true;
        obj->slideEventReady = false;
        obj->colorType = determineColor(type, beat);

        if (type == HitObjectEnums::DoubleTap ){
            obj->tapDelayDuration = duration;
        }

        updateObjectPoolIndex(obj);
        emit updateQml(obj);


    }

}

void NotePool::updateObjectPoolIndex(QSharedPointer<HitObject> object)
{

    switch(object->type){

    case HitObjectEnums::Tap:
    {
        if (tapNotePoolIndex + 1 >= tapNotes.size()){
            tapNotePoolIndex = 0;
            break;
        }
        tapNotePoolIndex++;

        break;
    }

    case HitObjectEnums::DoubleTap:
    {
        if (doubleTapNotePoolIndex + 1 >= doubleTapNotes.size()){
            doubleTapNotePoolIndex = 0;
            break;
        }
        doubleTapNotePoolIndex++;
        break;
    }
    case HitObjectEnums::Hold:
    {
        if (holdNotePoolIndex + 2 >= holdNotes.size()){
            holdNotePoolIndex = 0;
            break;
        }
        holdNotePoolIndex++;
        break;
    }
    case HitObjectEnums::Slide:
    {
        if (holdNotePoolIndex + 1 >= holdNotes.size()){
            holdNotePoolIndex = 0;
            break;
        }
        holdNotePoolIndex++;
        break;
    }
    case HitObjectEnums::Forgotten:
    {
        if (forgottenNotePoolIndex + 1 >= forgottenNotes.size()){
            forgottenNotePoolIndex = 0;
            break;
        }
        forgottenNotePoolIndex++;
        break;
    }
    default:break;
    }

}

HitObjectEnums::ColorTypes NotePool::determineColor(HitObjectEnums::Types type, float beat)
{
    if (type == HitObjectEnums::Hold ){
        return HitObjectEnums::HoldNote;
    }
    else     if (type == HitObjectEnums::Forgotten ){
        return HitObjectEnums::ForgottenNote;
    }

    else if (type == HitObjectEnums::DoubleTap ){
        return HitObjectEnums::DoubleTapNote;
    }
    else if (type == HitObjectEnums::Slide ){
        return HitObjectEnums::SlideNote;
    }
    else if (type == HitObjectEnums::Tap ){

        //Beat consists of beat + totalOffset. We subtract to get the base beat of this note
        float baseBeat = beat - conductor->totalOffsetBeats;
        float remainder = std::fmod(baseBeat, std::trunc(baseBeat));
        if(remainder == 0){
            return HitObjectEnums::QuarterNote;
        }
        else if (remainder == 0.5)
        {
            return HitObjectEnums::EighthNote;
        }
        else if ( QString::number(remainder, 'f', 1) == "0.3" ||QString::number(remainder, 'f', 1) == "0.7"){
            return HitObjectEnums::TripletNote;
        }
        else{
            return HitObjectEnums::QuarterNote;
        }

    }
    return HitObjectEnums::QuarterNote;;
}

void NotePool::calculateSlideNoteHeadXPosition( QSharedPointer<HitObject> object)
{
    float travelDistance = qAbs(object->tailObject->yPosition - judgementLine->yPosition());
    float xProgress = 1.0f - (travelDistance / object->tailObject->totalTravelDistance);
    object->xPosition =  std::lerp(object->laneXPos, object->tailObject->xPosition,  xProgress);
    emit updateQml(object);

}

void NotePool::holdActivated( QSharedPointer<HitObject>object)
{

    object->holdActivated = true;
    object->tailObject->holdActivated = true;
    object->tickSoundBeat = conductor ? conductor->songPosInBeats : 0;
    object->visible = false;
    if (object->type ==  HitObjectEnums::Hold){ //If slide note, we wait until the controller is ready to spawn emitter

        emit spawnedGimmickObjChanged(object->objectPoolIndex);
        emit spawnHoldEmitter(object->lane - 1);
    }
    emit updateQml(object);
}

void NotePool::holdActive( QSharedPointer<HitObject>object)
{

    object->yPosition = judgementLine->yPosition();
    if(conductor && conductor->songPosInBeats > object->tickSoundBeat)
    {
        object->tickSoundBeat = conductor->songPosInBeats;
        playHitSoundC();
    }

    if (object->type ==  HitObjectEnums::Slide && object->tailObject) {

        calculateSlideNoteHeadXPosition(object);
    }
    emit updateQml(object);
}

void NotePool::holdNoteTailObject( QSharedPointer<HitObject>object, float index, float spawnY, QVector<QSharedPointer<HitObject>> holdNotes)
{

    tailNoteMovement(object, spawnY);

    if ( object->noteActive){
        if(object->holdActivated && object->yPosition  - centerOffset >= judgementLine->yPosition() - centerOffset) //this should be judgementypos + judgementheight/2
        {

            //Head object should be valid or we crash to avoid possible leaks
            emit despawnTrail(object->headObject->objectPoolIndex);
            resetNote(object);
            return;
        }

        if(object->yPosition > windowHeight + 1)
        {

            emit despawnTrail(object->headObject->objectPoolIndex);
            resetNote(object);

            return;
        }
    }

}

void NotePool::holdNoteHeadObject( QSharedPointer<HitObject> object, int index, float validMs, float beatOffset)
{
    //Destroy when tail doesn't exist
    if (object->tailObject == nullptr || !object->tailObject->noteActive){

        if (object->holdActivated){
            playHitSoundB();
            emit noteHit("Perfect", object->colorType, object->xPosition, hitObjectModel->noteWidth());
        }
        resetNote(object);

        return;
    }

    if (object->holdActivated){

        if (object->type == HitObjectEnums::Slide){
            if (!object->slideEventReady && centerMouse() < 2){
                object->slideEventReady = true;
                relativeMouseX = 0;
                slideValue = 0;

                emit setRelativeMouseX(relativeMouseX);
                emit spawnedGimmickObjChanged(object->objectPoolIndex);
            }
            else if (object->slideEventReady) {
                QRect windowRect = windowObj->geometry(); // variable since screen size can change
                float mouseX = QCursor::pos().x();

                //Ensure mouse is within game window
                if(mouseX > windowRect.left() && mouseX < windowRect.right()){
                    relativeMouseX = (mouseX - mouseStartX) * 1;
                    emit setRelativeMouseX(relativeMouseX);
                }


            }
        }


        holdActive(object);
        //Player let go of button?
        if(!laneHeldStatuses[object->lane - 1])
        {
            holdFailed(index,object);
        }
    }

    if (!object->holdFailed && !object->holdActivated){

        if(lanePressedStatuses[object->lane - 1] && !laneJudgementActiveStatuses[object->lane - 1] && !object->missed && holdNoteActivationCheck(object, object->tailObject, object->yPosition, object->lane, beatOffset)){
            holdActivated(object);
            return;
        }

        if (!object->missed && object->yPosition > judgementLine->yPosition()  - centerOffset && validMs >= belowLineMissMs)
        {
            noteMissed(object);
            return;
        }
    }

}

bool NotePool::holdNoteActivationCheck(QSharedPointer<HitObject> obj, QSharedPointer<HitObject> tailObject, float yPos, int lane, float beatOffset)
{

    float hitMs = conductor ? beatOffset * (conductor->secsPerBeat * 1000) : 1000;

    if (hitMs <= missMs){
        noteJudgement = "Perfect";
        tailObject->totalTravelDistance = qAbs(tailObject->yPosition - judgementLine->yPosition());
        return true;
    }
    return false;

}

void NotePool::holdFailed(int objIndex, QSharedPointer<HitObject>obj)
{
    if (obj == nullptr){
        obj = hitObjectModel->objects[objIndex];
    }

    if (obj->tailObject){
        obj->tailObject->holdActivated = false;
        obj->tailObject->holdFailed = true;
    }

    obj->holdActivated = false;
    obj->holdFailed = true;
    noteJudgement = "Miss";

    emit updateQml(obj);
    emit noteMiss(noteJudgement, obj->lane);
}

void NotePool::tailNoteMovement( QSharedPointer<HitObject>object, float spawnY)
{
    if(conductor){
        double hitBeatTime =  (object->beat + object->noteHoldDuration) * conductor->secsPerBeat;
        double spawnBeatTime = hitBeatTime - (beatsUntilHit * conductor->secsPerBeat);
        double progress = (conductor->currentTime - spawnBeatTime)/(beatsUntilHit * conductor->secsPerBeat);
        object->yPosition =  std::lerp(spawnY, judgementLine->yPosition()  - centerOffset, progress);


        emit updateQml(object);
    }
}

void NotePool::doubleTapNoteObject (QSharedPointer<HitObject>object, int noteIndex, float validMs, float beatOffset)
{
    if(!object->secondTapActive)
    {
        if (lanePressedStatuses[object->lane - 1] && !laneJudgementActiveStatuses[object->lane - 1]){
            if ( validMs < missMs && (judgementLine->yPosition()  - centerOffset) || validMs < belowLineMissMs && object->yPosition > (judgementLine->yPosition()  - centerOffset) ){
                object->secondTapActive = true;
                object->yPosition = judgementLine->yPosition()  - centerOffset;
                emit updateQml(object);
                emit spawnedGimmickObjChanged(object->objectPoolIndex);
            }
        }
    }
    else{

        if(conductor){
            object->secondTapBeatTime =  (object->beat + object->tapDelayDuration) * conductor->secsPerBeat;
            float secondTapStartTime = object->secondTapBeatTime - (object->tapDelayDuration * conductor->secsPerBeat);
            float secondHitProgress = (conductor->currentTime - secondTapStartTime)/(object->tapDelayDuration * conductor->secsPerBeat);
            object->secondTapJudgementScale = std::lerp(object->doubleTapMaxScale, 1,qMin(secondHitProgress, 1.0));

            emit updateQml(object);
            emit updateSecondTapRingScale(object->secondTapJudgementScale, secondHitProgress);

            if(lanePressedStatuses[object->lane - 1] && !laneJudgementActiveStatuses[object->lane - 1]){
                //Check if note hit!
                judgement(object, beatOffset, noteIndex, true);
            }

            if (conductor->currentTime > object->secondTapBeatTime + (0.2 *  conductor->secsPerBeat)){
                noteMissed(object);
                resetNote(object);
            }
        }

    }

}

void NotePool::noteYPosition( QSharedPointer<HitObject> object, float validMs,float spawnY)
{
    if(conductor){
        double hitBeatTime =  object->beat * conductor->secsPerBeat;
        double spawnBeatTime = hitBeatTime - (beatsUntilHit * conductor->secsPerBeat);
        double progress = (conductor->currentTime - spawnBeatTime)/(beatsUntilHit * conductor->secsPerBeat);

        if(!object->holdActivated)
        {
            object->yPosition =  std::lerp(spawnY, judgementLine->yPosition()  - centerOffset, progress);
            emit updateQml(object);
        }

        if (object->type != HitObjectEnums::Forgotten && !object->missed && object->yPosition > (judgementLine->yPosition()  - centerOffset) &&  validMs >= belowLineMissMs )
        {
            noteMissed(object);
            return;
        }
    }
}

float NotePool::getLaneXPosition(int lane)
{
    laneCenter = laneUnit/2; //The value it takes to reach center (of the lane) from the left side of the lane
    float spacing = laneUnit * (lane - 1);
    return (laneUnit/2 - hitObjectModel->noteWidth()/2) + spacing;

}

void NotePool::noteMissed( QSharedPointer<HitObject>object)
{
    object->missed = true;
    noteJudgement = "Miss";
    emit noteMiss(noteJudgement, object->lane);
}

QString NotePool::judgement(QSharedPointer<HitObject> object,float distance, int index, bool destroyNote)
{

    float hitMs = conductor ? distance * conductor->secsPerBeat * 1000 : 0;

    if(object->objectPoolIndex == 1){
    }

    if(object->type == HitObjectEnums::DoubleTap ){
        missMs *= .7;
    }

    if (object->type == HitObjectEnums::Tap ){

        if( hitMs < greatMs ){
            noteJudgement = "Perfect";
            if (destroyNote){

                emit noteHit(noteJudgement, object->colorType, object->xPosition, hitObjectModel->noteWidth());
                playHitSoundA();

                laneJudgementActiveStatuses[object->lane - 1] = true;
                lanePressedStatuses[object->lane - 1] = false;
                resetNote(object);

                return "destroyed";

            }
            else
                return "passed";
        }
        else if (hitMs > greatMs  && hitMs < missMs){
            noteJudgement = "Good";
            if (destroyNote){

                emit noteHit(noteJudgement, object->colorType, object->xPosition,hitObjectModel->noteWidth());
                laneJudgementActiveStatuses[object->lane - 1] = true;
                lanePressedStatuses[object->lane - 1] = false;
                playHitSoundA();
                resetNote(object);
                return "destroyed";

            }
            else
                return "passed";
        }
    }
    else if (object->type == HitObjectEnums::DoubleTap ) {
        float accuracy = conductor ? qAbs((conductor->currentTime - object->secondTapBeatTime) * (conductor->secsPerBeat * 1000)) : 0;
        missMs = 83.33;
        greatMs = 32.33;


        if (accuracy < greatMs){
            noteJudgement = "Perfect";

            if (destroyNote){
                emit noteHit(noteJudgement, object->colorType, object->xPosition,hitObjectModel->noteWidth());
                playHitSoundB();
                laneJudgementActiveStatuses[object->lane - 1] = true;
                lanePressedStatuses[object->lane - 1] = false;
                resetNote(object);
                updateAccuracyMilliseconds();
                return "destroyed";
            }
        }
        else{
            noteJudgement = "Good";

            if (destroyNote){
                emit noteHit(noteJudgement,object->colorType, object->xPosition,hitObjectModel->noteWidth());
                playHitSoundB();
                laneJudgementActiveStatuses[object->lane - 1] = true;
                lanePressedStatuses[object->lane - 1] = false;
                resetNote(object);
                updateAccuracyMilliseconds();
                return "destroyed";
            }
        }

    }
    else if (object->type == HitObjectEnums::Forgotten ){
        if ( hitMs < (missMs/2) && object->yPosition - centerOffset <= judgementLine->yPosition() - centerOffset){
            noteJudgement = "Forgotten";
            if (destroyNote){

                emit noteHit(noteJudgement, object->colorType, object->xPosition, hitObjectModel->noteWidth() );
                laneJudgementActiveStatuses[object->lane - 1] = true;
                lanePressedStatuses[object->lane - 1] = false;
                playHitSoundD();
                resetNote(object);

                return "destroyed";
            }
            else
                return "passed";
        }
    }

    return "";
}

void NotePool::atScreenBottomCheck( QSharedPointer<HitObject> object, int index )
{
    if (object->yPosition + centerOffset > windowHeight + boundaryOffset){
        resetNote(object);
    }
}

void NotePool::resetNote(QSharedPointer<HitObject>object)
{

    object->holdActivated = false;
    object->holdFailed = false;
    object->totalTravelDistance = 0;
    object->missed = false;
    object->xPosition = 0;
    object->slideEventReady = false;
    object->yPosition = 0 - boundaryOffset;
    object->secondTapActive = false;
    object->noteActive = false;
    object->beat =  0;
    object->lane = 1;
    object->type = HitObjectEnums::Tap;
    object->colorType = HitObjectEnums::QuarterNote;
    object->noteHoldDuration = 0;
    object->tailNote = false;
    object->tailObject = nullptr;
    object->visible = false;
    emit updateQml(object);
}

float NotePool::centerMouse()
{
    QRect windowRect = windowObj->geometry();
    QPoint center = windowRect.center();
    QCursor::setPos(center);

    //Because the function is run until mouse centers, our final value should be the center of the screen
    mouseStartX = center.x();
    return qAbs(QCursor::pos().x() - center.x());

}

void NotePool::updateAccuracyMilliseconds()
{
    missMs = 383.083;
    belowLineMissMs = 122.33;
    greatMs = 108.66;
}

void NotePool::deleteHitSoundRefs()
{
    for (int i = 0; i < maSoundList.length();){
        ma_sound * obj = maSoundList.at(i);
        if(ma_sound_at_end(obj)){
            ma_sound_uninit(obj);
            delete obj;
            maSoundList.removeAt(i);
        }
        else
            /*As list shrinks we could be skipping over an object, so increment here*/
            ++i;
    }
}

void NotePool::playHitSoundA()
{
    ma_sound* hitsound = new ma_sound;

    ma_result result =  ma_sound_init_copy(&audioHandler->engine, &audioHandler->hitSoundA, MA_SOUND_FLAG_DECODE ,NULL, hitsound);
    if (result != MA_SUCCESS){
        delete hitsound;
    }
    else
    {
        ma_sound_set_volume(hitsound, sfxVolume);
        maSoundList.append(hitsound);
        ma_sound_start(hitsound);
    }
}

void NotePool::playHitSoundB()
{
    ma_sound* hitsound = new ma_sound;

    ma_result result =  ma_sound_init_copy(&audioHandler->engine, &audioHandler->hitSoundB, MA_SOUND_FLAG_DECODE ,NULL, hitsound);
    if (result != MA_SUCCESS){
        delete hitsound;
    }
    else
    {
        ma_sound_set_volume(hitsound, sfxVolume);
        maSoundList.append(hitsound);
        ma_sound_start(hitsound);
    }
}

void NotePool::playHitSoundC()
{
    ma_sound* tickSound = new ma_sound;

    ma_result result =   ma_sound_init_copy(&audioHandler->engine, &audioHandler->hitSoundC, MA_SOUND_FLAG_DECODE ,NULL, tickSound);
    if (result != MA_SUCCESS){
        delete tickSound;
    }
    else
    {
        ma_sound_set_volume(tickSound, sfxVolume);
        maSoundList.append(tickSound);
        ma_sound_start(tickSound);
    }
}

void NotePool::playHitSoundD()
{
    ma_sound* hitsound = new ma_sound;

    ma_result result =  ma_sound_init_copy(&audioHandler->engine, &audioHandler->hitSoundD, MA_SOUND_FLAG_DECODE ,NULL, hitsound);
    if (result != MA_SUCCESS){
        delete hitsound;
    }
    else
    {
        ma_sound_set_volume(hitsound, sfxVolume);
        maSoundList.append(hitsound);
        ma_sound_start(hitsound);
    }
}

float NotePool::getScreenTop()
{
    return screenGeometry.top();
}

float NotePool::getScreenBottom()
{
    return screenGeometry.bottom();

}

QString NotePool::noteType() const
{
    return m_noteType;
}

void NotePool::setNoteType(const QString &newNoteType)
{
    if (m_noteType == newNoteType)
        return;
    m_noteType = newNoteType;
    emit noteTypeChanged();
}


void NotePool::clearPool()
{
    gameEnd = true;
    emit clearModel();
    tapNotes.clear();
    doubleTapNotes.clear();
    holdNotes.clear();
    forgottenNotes.clear();
    deleteHitSoundRefs();
}
