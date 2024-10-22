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

class chatRoom;
class MainWindow;
class messageSender;

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
    //explicit ChatServer(QObject *parent = nullptr);

    // 싱글톤 인스턴스 가져오기
    static ChatServer& instance() {
        static ChatServer instance;
        return instance;
    }
    void setMainWindow(MainWindow* window);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

    void processSignupRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void processLoginRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void sendResponse(QTcpSocket *clientSocket, const QString &status, const QString &message) const;
    void sendResponse(QTcpSocket *clientSocket, const QJsonObject &responseJson) const;

signals:
    void AddUser(USER *user);       // 사용자 추가 시 발생하는 시그널
    void DisconnectUser(USER *user); // 사용자 연결 해제 시 발생하는 시그널
    void messageReceived(const QString &id, const QString &message);
    void sendMessageToClient(QString &msg);

private:
    MainWindow *mainwindow;
    QMap<QTcpSocket*, USER*> clientMap;
    QList<QThread*> threadList;
    QMap<messageSender*, USER*> senderList; // sender와 user를 매핑하여 관리

    // 싱글톤이므로 생성자를 private으로 제한
    ChatServer(QObject *parent = nullptr);
    ~ChatServer() {}

    // 복사 및 대입을 금지
    ChatServer(const ChatServer&) = delete;
    ChatServer& operator=(const ChatServer&) = delete;
};
#endif // CHATSERVER_H
