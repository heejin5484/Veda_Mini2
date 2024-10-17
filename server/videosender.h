#ifndef VIDEOSENDER_H
#define VIDEOSENDER_H
#include <QTcpSocket>
#include "chatserver.h"

class VideoSender : public QObject
{
    Q_OBJECT
public:
    explicit VideoSender();
    ~VideoSender();
    void setSocket(QTcpSocket* socket);

public slots:
    void sendImage(const QByteArray& image);
    void onReadyRead();  // 데이터 수신
    void onDisconnected();  // 클라이언트 연결 종료 시 처리

signals:
    void ProcessData(QByteArray data, USER* user);

private:
    QTcpSocket *clientSocket;  // 클라이언트 소켓
    qintptr socketDescriptor;  // 소켓 디스크립터
    USER *user;
    bool isUserAuthenticated;

    //ChatServer 싱글톤 참조
    ChatServer& chatserver;
};

#endif // VIDEOSENDER_H
