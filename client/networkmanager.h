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

signals:
    void responseReceived(bool success, const QString &response); // 성공 여부와 메시지를 전달

private:
    QTcpSocket *socket;

private slots:
    void onReadyRead();  // 서버 응답을 읽어들이는 함수
};

#endif // NETWORKMANAGER_H
