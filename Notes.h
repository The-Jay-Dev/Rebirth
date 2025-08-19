#ifndef NOTES_H
#define NOTES_H
#include "HitObject.h"
#include "HitObjectEnums.h"
#include <QString>

struct Note{
    float beat = 0.0;
    float duration = 0.0;
    int lane = -1;
    HitObjectEnums::Types type;
    int tailLane = -1;
};

#endif // NOTES_H
