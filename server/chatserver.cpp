#include "chatserver.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QBuffer>
//#include "clientthread.h"
#include "videosender.h"

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr)
{
/*    mainwindow = static_cast<MainWindow*>(parent);

    // mainwindow와 연결
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
*/
}

void ChatServer::setMainWindow(MainWindow* window)
{
    mainwindow = window;

    // MainWindow가 설정된 후에 연결을 설정합니다.
    if (mainwindow != nullptr) {
        connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
        connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);
        connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
    } else {
        qDebug() << "MainWindow is nullptr, connections not made.";
    }
    qDebug() << "Current thread: " << QThread::currentThread();
    qDebug() << "chatserver thread" << this->thread();
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {

    // 소켓을 메인 스레드에서 생성
    QTcpSocket* clientSocket = new QTcpSocket();
    if (!clientSocket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error: Could not set socket descriptor";
        delete clientSocket;
        return;
    }

    // 새로운 스레드를 생성
    QThread* thread = new QThread;
    VideoSender* sender = new VideoSender();
    sender->moveToThread(thread); // sender에서 QObject를 상속받을때 public으로 상속받았어야함..
    // 소켓을 스레드로 이동
    clientSocket->moveToThread(thread);
    sender->setSocket(clientSocket);

    // client 접속 끊기면 thread 종료하는거도 넣기

    // clientThread에서 유저 정보를 받을 수 있게 신호 연결
    connect(this, &ChatServer::sendImageToClient, sender, &VideoSender::sendImage);
    connect(sender, &VideoSender::disconnected, thread, &QThread::quit);
    connect(thread,&QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

// 유저 정보와 스레드를 clientMap에 추가하는 메서드
void ChatServer::addClientToMap(VideoSender* sender, USER* user) {
    clientMap.insert(sender, user);  // 스레드와 유저를 clientMap에 추가
}

void ChatServer::removeClient(VideoSender* sender) {
    // clientThread에서 해당하는 유저 정보를 가져옴
    USER* user = clientMap.value(sender);

    if (user) {
        qDebug() << "Removing user:" << user->ID;
        emit DisconnectUser(user);  // UI에 유저 삭제 신호 보냄
    }

    // clientMap에서 videosender 삭제
    clientMap.remove(sender);
    sender->deleteLater();
}

void ChatServer::onImageCaptured(int id, const QImage &image) {
    Q_UNUSED(id);

    if (!clientMap.isEmpty()){
        // QImage -> JPEG
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "JPEG", 50);  // QImage를 JPEG로, 압축품질 50

        // 이미지 데이터를 전송 큐에 추가
        QMutexLocker locker(&imageMutex);  // 큐 동기화
        imageQueue.enqueue(byteArray);     // 전송 큐에 이미지 추가
        locker.unlock();

        // 큐에 저장된 데이터를 처리하도록 호출
        broadcastImage();
    }
}

void ChatServer::enqueueMessage(const QByteArray& message){
    QMutexLocker locker(&messageMutex);
    messageQueue.enqueue(message);
}

void ChatServer::enqueueImage(const QByteArray& imageData){
    QMutexLocker locker(&imageMutex);
    imageQueue.enqueue(imageData);
}

void ChatServer::broadcastMessage(){
    /*QMutexLocker locker(&messageMutex);
    while (!messageQueue.isEmpty()) {
        QByteArray message = messageQueue.dequeue();
        foreach(QTcpSocket* client, clientMap.keys()) {
            client->write(message);
            client->flush();
        }
    }
*/
}

void ChatServer::broadcastImage(){
    QMutexLocker locker(&imageMutex);  // 큐 동기화

    if (clientMap.isEmpty()) {
        qDebug() << "No clients connected, skipping broadcast.";
        imageQueue.clear();  // 클라이언트가 없으면 큐를 비움
        return;  // 클라이언트가 없으면 그냥 리턴
    }

    qDebug() << "Clients connected: " << clientMap.size();

    // 각 클라이언트의 스레드 상태를 체크하고 이미지 전송
    foreach (VideoSender* sender, clientMap.keys()) {
        // if (!sender->isRunning()) {
        //     qDebug() << "Client thread is not running, skipping...";
        //     continue;
        // }
        qDebug() << " ==============";
        qDebug() << sender->thread();
        qDebug() << QThread::currentThread();
        qDebug() << "===============";

        // 이미지 큐에서 이미지를 가져옴
        while (!imageQueue.isEmpty()) {
            QByteArray byteArray = imageQueue.dequeue();  // 큐에서 이미지 가져오기

            // 이미지 전송 신호를 스레드로 보냄 이렇게하니까오류났음
            emit sendImageToClient(byteArray);  // 이미지를 스레드로 전송
            //QMetaObject::invokeMethod(clientThread, "sendImage", Qt::QueuedConnection, Q_ARG(QByteArray, byteArray));
            // QMetaObject::invokeMethod로 해당 클라이언트 스레드에서 sendImage 호출

            qDebug() << "Image sent to clients.";
        }
    }
}
