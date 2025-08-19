#ifndef HITOBJECTMODEL_H
#define HITOBJECTMODEL_H

#include "HitObject.h"
#include "MessageHandler.h"
#include "HitObjectEnums.h"
#include "NotePool.h"
#include <QAbstractListModel>
#include <QObject>
#include <QRect>

class HitObjectModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(float noteWidth READ noteWidth WRITE setNoteWidth NOTIFY noteWidthChanged FINAL)
    Q_PROPERTY(float noteTrackWidth READ noteTrackWidth WRITE setNoteTrackWidth NOTIFY noteTrackWidthChanged FINAL)
    Q_PROPERTY(float relativeMouseX READ relativeMouseX WRITE setRelativeMouseX NOTIFY relativeMouseXChanged FINAL)

public:
    explicit HitObjectModel(QObject *parent = nullptr,MessageHandler* msgHandler = nullptr, NotePool* nPool = nullptr);

    ~HitObjectModel();
    MessageHandler* messageHandler;
    QList<QSharedPointer<HitObject>> objects;

    void clearModel();

    float noteTrackWidth() const;
    void setNoteTrackWidth(float newNoteTrackWidth);

    float noteWidth() const;

    float relativeMouseX() const;


public slots:
    void addToObjectPool(QSharedPointer<HitObject> object);
      void updateQml(QSharedPointer<HitObject> object);
    void alert(QString msg, int messageStatus = 2);
    void setRelativeMouseX(float newRelativeMouseX);
    void setNoteWidth(float newNoteWidth);
    void spawnTrailSlot(int index);
    void despawnTrailSlot(int index);
    void spawnHoldEmitterSlot(int lane);
    void spawnedGimmickObjChangedSlot(int index);
    void despawnHoldEmitterSlot(int lane);
    void noteHitSlot(QString noteJudgement, HitObjectEnums::ColorTypes colorType, float xPosition, float objectWidth );
    void updateSecondTapRingScaleSlot(float scale, float progress);


signals:
    void despawnHoldEmitter(int lane);
      void spawnedGimmickObjChanged(int index);
    void spawnTrail(int index);
    void despawnTrail(int index);
    void spawnHoldEmitter(int lane);
    void noteHit(QString noteJudgement, HitObjectEnums::ColorTypes colorType, float xPosition, float objectWidth );
            void updateSecondTapRingScale(float scale, float progress);


        void noteTrackWidthChanged();

        void noteWidthChanged();

        void relativeMouseXChanged();
        void holdFailed(int index,QSharedPointer<HitObject> object = nullptr );

    private:


    //Required to work the model
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum Types{
        Tap,
        DoubleTap,
        Hold,
        Slide,
        Forgotten

    };

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

    enum NoteType{
        Quarter,
        Eighth,
        Triplet
    };


    NotePool * notePool = nullptr;


    bool noteBatchUpdated = true;
    float m_noteTrackWidth = 0.0;
    float m_noteWidth = 0.0;
    float m_relativeMouseX = 0.0;
};

#endif // HITOBJECTMODEL_H
