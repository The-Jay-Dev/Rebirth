#include "HitObjectModel.h"
#include "NotePool.h"
#include <QCursor>
#include <QScreen>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>


HitObjectModel::HitObjectModel(QObject *parent, MessageHandler* msgHandler,  NotePool* nPool)
    : QAbstractListModel{parent}, messageHandler(msgHandler), notePool(nPool)
{
    connect(notePool, &NotePool::addToObjectPool, this, &HitObjectModel::addToObjectPool, Qt::QueuedConnection);
    connect(notePool, &NotePool::updateQml, this, &HitObjectModel::updateQml, Qt::QueuedConnection);
    connect(notePool, &NotePool::alert, this, &HitObjectModel::alert, Qt::QueuedConnection);
    connect(notePool, &NotePool::spawnedGimmickObjChanged, this, &HitObjectModel::spawnedGimmickObjChangedSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::spawnTrail, this, &HitObjectModel::spawnTrailSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::despawnTrail, this, &HitObjectModel::despawnTrailSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::spawnHoldEmitter, this, &HitObjectModel::spawnHoldEmitterSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::setRelativeMouseX, this, &HitObjectModel::setRelativeMouseX, Qt::QueuedConnection);
    connect(notePool, &NotePool::noteHit, this, &HitObjectModel::noteHitSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::updateSecondTapRingScale, this, &HitObjectModel::updateSecondTapRingScaleSlot, Qt::QueuedConnection);
    connect(notePool, &NotePool::clearModel, this, &HitObjectModel::clearModel, Qt::QueuedConnection);
}

HitObjectModel::~HitObjectModel()
{
    clearModel();
    messageHandler = nullptr;
}

void HitObjectModel::addToObjectPool(QSharedPointer<HitObject> object)
{
    beginInsertRows(QModelIndex(),objects.size(),objects.size());
    objects.append(object);
    endInsertRows();

    emit dataChanged(index(object->objectPoolIndex),
                     index(object->objectPoolIndex), {YRole,XRole,Lane, ColorType,
                      HoldFailed, JudgementRingMaxScale, DebugText,
                      SlideEventReady});
}

void HitObjectModel::updateQml(QSharedPointer<HitObject> object)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);
    emit dataChanged(index(object->objectPoolIndex),
                     index(object->objectPoolIndex), {
                      XRole,
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
                      SlideEventReady});



}

void HitObjectModel::alert(QString msg, int messageStatus)
{
    MessageHandler::Status status = MessageHandler::INFO;
    switch (messageStatus) {
    case 0:
        status = MessageHandler::SUCCESS;
        break;
    case 1:
        status = MessageHandler::FAILED;
        break;
    case 2:
        status = MessageHandler::INFO;
        break;
    default:
        break;
    }
    emit messageHandler->logMessage(msg, status);
}


int HitObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return objects.size();
}

//Binding a QML-readable name to each of the enums
QHash<int, QByteArray> HitObjectModel::roleNames() const
{
    static QHash<int, QByteArray> mapping {
        {XRole, "xPosition"},
        {YRole, "yPosition"},
        {Lane, "lane"},
        {Beat, "beat"},
        {Type, "type"},
        {Active, "noteActive"},
        {ColorType, "colorType"},
        {TailNote, "tailNote"},
        {HoldFailed, "holdFailed"},
        {JudgementRingMaxScale, "judgementRingMaxScale"},
        {SlideEventReady, "slideEventReady"},
        {DebugText, "debugText"},
        {Visible, "noteVisible"},
        {TailNoteXRole, "tailNoteXPosition"},
        {TailNoteYRole, "tailNoteYPosition"}
    };
    return mapping;
}

//Called when QML attempts to access one of your roles (e.g. xPosition)
QVariant HitObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= objects.size())
        return QVariant();

    const  QSharedPointer<HitObject> hitObj = objects.at(index.row());

    switch (role) {
    case XRole: return hitObj->xPosition;
    case YRole: return hitObj->yPosition;
    case Lane: return hitObj->lane;
    case Beat: return hitObj->beat;
    case Type: return hitObj->type;
    case Active: return hitObj->noteActive;
    case ColorType: return hitObj->colorType;
    case TailNote: return hitObj->tailNote;
    case HoldFailed: return hitObj->holdFailed;
    case JudgementRingMaxScale: return hitObj->doubleTapMaxScale;
    case SlideEventReady: return hitObj->slideEventReady;
    case DebugText: return hitObj->debugText;
    case Visible: return hitObj->visible;
    case TailNoteXRole: return hitObj->tailObject ?  hitObj->tailObject->xPosition : 0;
    case TailNoteYRole: return hitObj->tailObject ?   hitObj->tailObject->yPosition : 0;
    default: return QVariant(); //Safety net in case none of the above are returned
    }
}

void HitObjectModel::clearModel()
{
    beginResetModel();
    objects.clear();
    endResetModel();
}



float HitObjectModel::noteTrackWidth() const
{
    return m_noteTrackWidth;
}

void HitObjectModel::setNoteTrackWidth(float newNoteTrackWidth)
{
    if (qFuzzyCompare(m_noteTrackWidth, newNoteTrackWidth))
        return;
    m_noteTrackWidth = newNoteTrackWidth;
    emit noteTrackWidthChanged();
}

float HitObjectModel::noteWidth() const
{
    return m_noteWidth;
}

void HitObjectModel::setNoteWidth(float newNoteWidth)
{
    if (qFuzzyCompare(m_noteWidth, newNoteWidth))
        return;
    m_noteWidth = newNoteWidth;
    emit noteWidthChanged();
}

void HitObjectModel::spawnTrailSlot(int index)
{
    emit spawnTrail(index);
}

void HitObjectModel::despawnTrailSlot(int index)
{
    emit despawnTrail(index);
}

void HitObjectModel::spawnHoldEmitterSlot(int lane)
{
    emit spawnHoldEmitter(lane);
}

void HitObjectModel::spawnedGimmickObjChangedSlot(int index)
{
    emit spawnedGimmickObjChanged(index);
}

void HitObjectModel::despawnHoldEmitterSlot(int lane)
{
    emit despawnHoldEmitter(lane);
}

void HitObjectModel::noteHitSlot(QString noteJudgement, HitObjectEnums::ColorTypes colorType, float xPosition, float objectWidth)
{
    emit noteHit(noteJudgement, colorType, xPosition, objectWidth);
}

void HitObjectModel::updateSecondTapRingScaleSlot(float scale, float progress)
{
    emit updateSecondTapRingScale(scale, progress);
}

float HitObjectModel::relativeMouseX() const
{
    return m_relativeMouseX;
}

void HitObjectModel::setRelativeMouseX(float newRelativeMouseX)
{
    if (qFuzzyCompare(m_relativeMouseX, newRelativeMouseX))
        return;
    m_relativeMouseX = newRelativeMouseX;
    emit relativeMouseXChanged();
}
