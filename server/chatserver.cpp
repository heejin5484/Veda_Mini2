#include "chatserver.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QBuffer>
//#include "clientthread.h"
#include "videosender.h"
#include "messagehandler.h"

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

void ChatServer::handleClientConnection(){
    // clientServer로부터 새로운 연결을 받아 소켓을 생성
    QTcpSocket* clientSocket = clientServer->nextPendingConnection();

    if (!clientSocket) {
        qDebug() << "Error: Could not retrieve pending connection";
        return;
    }
    qDebug() << "New client connected for messaging.";

    // JSON 데이터 읽기
    if (clientSocket->waitForReadyRead(1000)) {
        QByteArray data = clientSocket->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        qDebug() << jsonObj;

        if (jsonObj["type"].toString() == "message") {
            // 비디오 소켓 처리
            handleMessageSocket(clientSocket);
        }
        else {
            qDebug() << "Unknown socket type";
            clientSocket->disconnectFromHost();
            clientSocket->deleteLater();
        }
    }
    // 클라이언트로부터 데이터가 수신될 때 호출될 슬롯 연결
    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket]() {
        QByteArray clientData = clientSocket->readAll();
        qDebug() << "Received message data: " << clientData;
        // 여기서 메시지 데이터를 처리하는 로직 추가
    });

}

void ChatServer::handleVideoConnection(){
    // videoServer로부터 새로운 연결을 받아 소켓을 생성
    QTcpSocket* videoSocket = videoServer->nextPendingConnection();
    if (!videoSocket) {
        qDebug() << "Error: Could not retrieve pending connection";
        return;
    }
    qDebug() << "New client connected for video.";

    if (videoSocket->waitForReadyRead(1000)) {
        QByteArray data = videoSocket->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();
        qDebug() << jsonObj;

        if (jsonObj["type"].toString() == "video") {
            // 비디오 소켓 처리
            handleVideoSocket(videoSocket);
        } else {
            qDebug() << "Unknown socket type";
            videoSocket->disconnectFromHost();
            videoSocket->deleteLater();
        }
    }
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {

    qDebug() << "a" ;
}

void ChatServer::handleMessageSocket(QTcpSocket* clientSocket){
    qDebug() << "msg socket income";
    //MessageHandler* messageHandler = new MessageHandler();
    //messageHandler->setSocket(clientSocket);
}

void ChatServer::handleVideoSocket(QTcpSocket* socket){
    // 소켓의 부모 관계를 끊음
    socket->setParent(nullptr);

    // 비디오 전송용 소켓
    // 새로운 스레드를 생성
    qDebug() << "video socket income";
    QThread* videoThread = new QThread;
    VideoSender* videoSender = new VideoSender();
    videoSender->moveToThread(videoThread);
    // 소켓을 스레드로 이동
    socket->moveToThread(videoThread);

    videoSender->setSocket(socket);
    // clientThread에서 유저 정보를 받을 수 있게 신호 연결
    connect(this, &ChatServer::sendImageToClient, videoSender, &VideoSender::sendImage);
    connect(videoSender, &VideoSender::disconnected, videoThread, &QThread::quit);
    connect(videoThread,&QThread::finished, videoThread, &QThread::deleteLater);
    USER* user;
    addClientToMap(videoSender, user);
    videoThread->start();


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

    qDebug() << "captured";
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
        qDebug() << "------";
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

