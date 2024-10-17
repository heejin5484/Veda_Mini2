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
    //ClientThread* clientThread = new ClientThread(socketDescriptor, this);
    QThread* thread = new QThread;
    VideoSender* sender = new VideoSender();
    sender->moveToThread(thread); // sender에서 QObject를 상속받을때 public으로 상속받았어야함..
    // 소켓을 스레드로 이동
    clientSocket->moveToThread(thread);

    sender->setSocket(clientSocket);





    //connect(thread, &ClientThread::finished, thread, &QObject::deleteLater);
    // client 접속 끊기면 thread 종료하는거도 넣기

    // clientThread에서 유저 정보를 받을 수 있게 신호 연결
    connect(this, &ChatServer::sendImageToClient, sender, &VideoSender::sendImage);
    thread->start();

}


// 유저 정보와 스레드를 clientMap에 추가하는 메서드
void ChatServer::addClientToMap(VideoSender* sender, USER* user) {
    qDebug() << " welcome------------------------";
    clientMap.insert(sender, user);  // 스레드와 유저를 clientMap에 추가
}

void ChatServer::removeClient(VideoSender* sender) {
    // clientThread에서 해당하는 유저 정보를 가져옴
    USER* user = clientMap.value(sender);

    if (user) {
        qDebug() << "Removing user:" << user->ID;
        emit DisconnectUser(user);  // UI에 유저 삭제 신호 보냄
        delete user;  // 유저 객체 삭제
    }

    // clientMap에서 스레드 삭제
    clientMap.remove(sender);

    // 스레드 종료 및 삭제
    // if (clientThread->isRunning()) {
    //     clientThread->quit();  // 스레드를 종료
    //     clientThread->wait();  // 스레드가 종료될 때까지 대기
    // }
    //clientThread->deleteLater();  // 스레드 객체 삭제
}

void ChatServer::onImageCaptured(int id, const QImage &image) {
    Q_UNUSED(id);

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
        //qDebug() << "No clients connected, skipping broadcast.";
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

/*
void ChatServer::processMessage(const QByteArray &data, QTcpSocket *socket) {
    qDebug() << "Received:" << data;

    // 데이터를 줄 단위로 분리
    QList<QByteArray> messages = data.split('\n');
    for (const QByteArray &message : messages) {
        if (message.isEmpty()) continue; // 빈 메시지는 무시

        // JSON 문자열 파싱
        QJsonDocument doc = QJsonDocument::fromJson(message);
        if (doc.isNull()) {
            qDebug() << "Invalid JSON received";
            continue; // 다음 메시지로 진행
        }

        QJsonObject json = doc.object();
        QString type = json["type"].toString();

        // 메시지 타입에 따른 처리
        if (type == "L") {
            QString username = json["username"].toString();
            qDebug() << username << "logged in";

            // 응답 보내기
            QJsonObject response;
            response["type"] = "L";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "O") {
            QString username = json["username"].toString();
            qDebug() << username << "logged out";

            // 응답 보내기
            QJsonObject response;
            response["type"] = "O";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "J") {
            QString username = json["username"].toString();
            QString password = json["password"].toString();
            qDebug() << "User joined:" << username;

            // 응답 보내기
            QJsonObject response;
            response["type"] = "J";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "I") {
            QString admin = json["admin"].toString();
            QString target = json["target"].toString();
            qDebug() << admin << "invited" << target;

            // 응답 보내기
            QJsonObject response;
            response["type"] = "I";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "G") {
            QString admin = json["admin"].toString();
            QString target = json["target"].toString();
            qDebug() << admin << "withdrawal out" << target;

            // 응답 보내기
            QJsonObject response;
            response["type"] = "G";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "F") {
            QString filename = json["filename"].toString();
            QString filePath = json["filePath"].toString(); // 파일 경로를 가져옴

            QString directory = "/home/seyeung/Veda_Mini2/server/save/"; // 저장할 디렉토리 경로
            QDir dir(directory);

            // 디렉토리가 존재하지 않으면 생성
            if (!dir.exists()) {
                if (!dir.mkpath(directory)) { // 디렉토리 생성
                    qDebug() << "디렉토리 생성 실패: " << QString("디렉토리 경로: %1").arg(directory);
                    continue; // 디렉토리 생성에 실패하면 다음 메시지로 진행
                }
            }

            // 클라이언트가 보낸 파일 경로에서 파일을 읽어서 저장
            QFile sourceFile(filePath);
            if (sourceFile.exists()) {
                // 파일을 읽어오기
                if (sourceFile.open(QIODevice::ReadOnly)) {
                    QByteArray fileData = sourceFile.readAll(); // 파일 데이터를 읽음
                    sourceFile.close(); // 파일 닫기

                    // 파일을 지정된 디렉토리에 저장
                    QFile destinationFile(directory + filename);
                    if (destinationFile.open(QIODevice::WriteOnly)) {
                        destinationFile.write(fileData); // 파일 데이터를 쓰기
                        destinationFile.close(); // 파일 닫기
                        qDebug() << "파일 저장 성공: " << filename;
                    } else {
                        qDebug() << "파일 저장 실패: " << destinationFile.errorString();
                    }
                } else {
                    qDebug() << "파일 읽기 실패: " << sourceFile.errorString();
                }
            } else {
                qDebug() << "파일이 존재하지 않음: " << filePath;
            }
        } else {
            qDebug() << "Unknown message type";
        }
    }
}


void ChatServer::sendResponse(QTcpSocket *socket, const QJsonObject &response) {
    QJsonDocument doc(response);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write(data + "\n");
    socket->waitForBytesWritten(3000);
}
*/
