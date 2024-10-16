#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include "chatserver.h"

class ClientThread : public QThread
{
public:
    explicit ClientThread(qintptr socketDescriptor, QObject *parent = nullptr);
    ~ClientThread();
    void setSocket(QTcpSocket* socket);

protected:
    void run() override;  // 스레드에서 처리될 로직
public slots:
    void sendImage(const QByteArray& image);
private slots:
    void onReadyRead();  // 데이터 수신
    void onDisconnected();  // 클라이언트 연결 종료 시 처리

signals:
    void ProcessData(QByteArray data, USER* user);

private:
    QTcpSocket *clientSocket;  // 클라이언트 소켓
    qintptr socketDescriptor;  // 소켓 디스크립터
    USER *user;
    bool isUserAuthenticated;
};

#endif // CLIENTTHREAD_H
