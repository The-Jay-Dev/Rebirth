#include "JudgementLineModel.h"
#include <QDebug>
JudgementLineModel::JudgementLineModel(QObject *parent) {
}

float JudgementLineModel::yPosition() const
{
    return m_yPosition;
}

void JudgementLineModel::setYPosition(float newYPosition)
{
    if (qFuzzyCompare(m_yPosition, newYPosition))
        return;
    m_yPosition = newYPosition;
    emit yPositionChanged();
}
