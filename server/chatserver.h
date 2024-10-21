#ifndef CHATSERVER_H
#define CHATSERVER_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QThread>
#include <QList>
#include <QMap>
#include <QPair>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include "databasemanager.h"

class MainWindow;

typedef struct user{
    QString userid;
    QString password;
    QString name;
    QTcpSocket *usersocket;
}USER; // 논의 필요


class ChatServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

    // 사용자 가입 요청 처리
        void processSignupRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);

        // 사용자 로그인 요청 처리
        void processLoginRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);

        // 메시지 요청 처리
        void processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);

        // 클라이언트에 응답 전송
        void sendResponse(QTcpSocket *clientSocket, const QString &status, const QString &message) const;

        // 오버로드된 응답 전송 메서드
        void sendResponse(QTcpSocket *clientSocket, const QJsonObject &responseJson) const;

    signals:
        void AddUser(USER *user);       // 사용자 추가 시 발생하는 시그널
        void DisconnectUser(USER *user); // 사용자 연결 해제 시 발생하는 시그널

private:
    MainWindow *mainwindow;
    QMap<QTcpSocket*, USER*> clientMap;
    QList<QThread*> threadList;
};
#endif // CHATSERVER_H
