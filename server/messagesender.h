#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "chatserver.h"
#include <QObject>

class messageSender : public QObject {
    Q_OBJECT
public:
    MessageSender(QTcpSocket* clientSocket, USER* user, QObject* parent = nullptr)
        : QObject(parent), clientSocket(clientSocket), user(user) {}

    void sendMessage(const QString& message) {
        if (clientSocket && clientSocket->isOpen()) {
            QJsonObject json;
            json["userid"] = user->userid;
            json["message"] = message;
            QJsonDocument doc(json);
            clientSocket->write(doc.toJson(QJsonDocument::Compact) + "\n");
            clientSocket->flush();
            qDebug() << "메시지 전송됨:" << message;
        }
    }

private:
    QTcpSocket* clientSocket;
    USER* user;
};

#endif // MESSAGESENDER_H
