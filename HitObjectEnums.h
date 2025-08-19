#ifndef HITOBJECTENUMS_H
#define HITOBJECTENUMS_H
#include <QObject>

class HitObjectEnums : public QObject {

    Q_OBJECT
public:
    enum Types{
        Tap,
        DoubleTap,
        Hold,
        Slide,
        Forgotten

    };
    Q_ENUM(Types);
    enum ColorTypes{
        QuarterNote,
        EighthNote,
        TripletNote,
        DoubleTapNote,
        HoldNote,
        SlideNote,
        ForgottenNote
    };
    Q_ENUM(ColorTypes);
};

#endif // HITOBJECTENUMS_H
