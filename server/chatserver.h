#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QThread>

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
};

#endif // CHATSERVER_H
