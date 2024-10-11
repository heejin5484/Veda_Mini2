#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class ChatServer : public QTcpServer {
    Q_OBJECT

public:
    ChatServer(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void processMessage(const QByteArray &data, QTcpSocket *socket);
    void sendResponse(QTcpSocket *socket, const QJsonObject &response);
};

#endif // CHATSERVER_H
