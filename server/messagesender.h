#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "chatserver.h"
#include <QObject>
#include <QThread>

class messageSender : public QObject {
    Q_OBJECT
public:
    messageSender();
    ~messageSender();
    void setSocket(QTcpSocket* socket);

private slots:
    void onReadyRead();
    void onDisconnected();

public slots:
    void sendMessage(const QString msg);

private:
    QTcpSocket* clientSocket;
    USER* user;
    //ChatServer 싱글톤 참조
    ChatServer& chatServer;
};

#endif // MESSAGESENDER_H
