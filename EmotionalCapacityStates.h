#ifndef EMOTIONALCAPACITYSTATES_H
#define EMOTIONALCAPACITYSTATES_H
#include "Notes.h"
#include <QStringList>
struct EmotionalCapacityStates{
    QString state;
    QVector<Note> notes;
};

#endif // EMOTIONALCAPACITYSTATES_H
