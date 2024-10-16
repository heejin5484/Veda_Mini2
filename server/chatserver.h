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
#include <QMutex>
#include <QQueue>

class ClientThread;
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

    // 메시지 큐 / 이미지 큐에 데이터 추가
    void enqueueMessage(const QByteArray& message);
    void enqueueImage(const QByteArray& imageData);

    // 메시지 / 이미지를 클라이언트에게 브로드캐스트
    void broadcastMessage();
    void broadcastImage();

    void removeClient(ClientThread* clientThread);
    void addClientToMap(ClientThread* clientThread, USER* user);

public slots:
    void onImageCaptured(int id, const QImage &image);
/*
private slots:
    void onNewConnection();
    void processMessage(const QByteArray &data, QTcpSocket *socket);
    void sendResponse(QTcpSocket *socket, const QJsonObject &response);
*/
protected :
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void ProcessData(QByteArray data, USER* user);
    void AddUser(USER* user);
    void DisconnectUser(USER *user);
    void sendImageToClient(const QByteArray& image);

private:
    MainWindow *mainwindow;
    QMap<ClientThread*, USER*> clientMap; // 스레드와 USER를 매핑하여 관리

    QQueue<QByteArray> messageQueue;  // 메시지 전송 큐
    QQueue<QByteArray> imageQueue;    // 이미지 전송 큐
    QMutex messageMutex;              // 메시지 큐 락
    QMutex imageMutex;                // 이미지 큐 락

};

#endif // CHATSERVER_H
