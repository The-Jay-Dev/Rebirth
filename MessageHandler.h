#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QString>
class MessageHandler : public QObject{
    Q_OBJECT
public:
    explicit MessageHandler(QObject* parent = nullptr){};

    enum Status{
        SUCCESS,
        FAILED,
        INFO
    };

    Q_ENUM(Status)
signals:
     void logMessage(QString text, Status status);
};

#endif // MESSAGEHANDLER_H
