#ifndef JUDGEMENTLINEMODEL_H
#define JUDGEMENTLINEMODEL_H

#include <QObject>
class JudgementLineModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float yPosition READ yPosition WRITE setYPosition NOTIFY yPositionChanged FINAL)
public:
   explicit JudgementLineModel(QObject *parent = nullptr);

    float yPosition() const;
    void setYPosition(float newYPosition);
signals:
    void yPositionChanged();
private:
    float m_yPosition;
};

#endif // JUDGEMENTLINEMODEL_H
