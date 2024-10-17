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

private slots:
    void processSignupRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void processLoginRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void sendResponse(QTcpSocket *clientSocket, const QString &status, const QString &message);
    void sendResponse(QTcpSocket *clientSocket, const QJsonObject &responseJson);
signals:
    void AddUser(USER *user);
    void DisconnectUser(USER *user);

private:
    MainWindow *mainwindow;
    QMap<QTcpSocket*, USER*> clientMap;
    QList<QThread*> threadList;
};
#endif // CHATSERVER_H
