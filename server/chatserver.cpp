#include "chatserver.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QBuffer>
#include "clientthread.h"

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr)
{
    mainwindow = static_cast<MainWindow*>(parent);

    // mainwindow와 연결
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
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
    ClientThread* clientThread = new ClientThread(socketDescriptor, this);
    // 소켓을 스레드로 이동
    clientSocket->moveToThread(clientThread);

    clientThread->setSocket(clientSocket); // 소켓을 스레드에 전달

    connect(clientThread, &ClientThread::finished, clientThread, &QObject::deleteLater);
    // clientThread에서 유저 정보를 받을 수 있게 신호 연결
    connect(this, &ChatServer::sendImageToClient, clientThread, &ClientThread::sendImage);

    clientThread->start();
}


// 유저 정보와 스레드를 clientMap에 추가하는 메서드
void ChatServer::addClientToMap(ClientThread* clientThread, USER* user) {
    clientMap.insert(clientThread, user);  // 스레드와 유저를 clientMap에 추가
}

void ChatServer::removeClient(ClientThread* clientThread) {
    // clientThread에서 해당하는 유저 정보를 가져옴
    USER* user = clientMap.value(clientThread);

    if (user) {
        qDebug() << "Removing user:" << user->ID;
        emit DisconnectUser(user);  // UI에 유저 삭제 신호 보냄
        delete user;  // 유저 객체 삭제
    }

    // clientMap에서 스레드 삭제
    clientMap.remove(clientThread);

    // 스레드 종료 및 삭제
    if (clientThread->isRunning()) {
        clientThread->quit();  // 스레드를 종료
        clientThread->wait();  // 스레드가 종료될 때까지 대기
    }
    clientThread->deleteLater();  // 스레드 객체 삭제
}

void ChatServer::onImageCaptured(int id, const QImage &image) {
    qDebug() << "send";
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

    // 큐에 데이터가 있으면 클라이언트로 전송
    while (!imageQueue.isEmpty()) {
        QByteArray byteArray = imageQueue.dequeue();  // 큐에서 이미지 가져오기

        // 각 ClientThread에 이미지를 전송하도록 신호 보냄
        emit sendImageToClient(byteArray);  // 이미지 데이터를 모든 스레드로 전송

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
