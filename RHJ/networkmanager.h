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
    void signupResponse(bool success);  // 회원가입 응답 신호

public slots:
    void handleSignupResponse(bool success);  // 회원가입 응답 처리 함수

private:
    QTcpSocket *socket;
};

#endif // NETWORKMANAGER_H
