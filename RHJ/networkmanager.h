#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    void sendMessage(const QJsonObject &json);

private:
    QTcpSocket *socket;
};

#endif // NETWORKMANAGER_H
