#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QThread>
#include <QList>
#include <QMap>
#include <QPair>

class MainWindow;

typedef struct user{
    QString ID;
    QString PW;
    QString name;
    QTcpSocket *usersocket;
}USER; // 논의 필요


class ChatServer : public QTcpServer
{
    Q_OBJECT

public:
    ChatServer(QObject *parent = nullptr);

protected :
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void ProcessData(QByteArray data);
    void AddUser(USER* user);
    void DisconnectUser(USER *user);

private:
    MainWindow *mainwindow;
    QList<QThread*> threadList; // 스레드 목록 관리
    QMap<QTcpSocket*, USER*> clientMap; // 소켓과 USER를 매핑하여 관리
};

#endif // CHATSERVER_H
