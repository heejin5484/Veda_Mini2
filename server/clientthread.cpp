#include "clientthread.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "chatserver.h"

ClientThread::ClientThread(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor), clientSocket(nullptr), isUserAuthenticated(false) {}


ClientThread::~ClientThread() {

    // delete user; 찾아보기 여기서 삭제하는게 적절한지..
}

void ClientThread::run() {
    // clientSocket은 이미 ChatServer에서 생성되고 설정된 상태
    if (clientSocket == nullptr) {
        qDebug() << "Error: No valid socket passed to thread";
        return;
    }

    qDebug() << "..";
    // 스레드로 소켓을 이동
    clientSocket->moveToThread(this);
    qDebug() << "here";
    // 소켓 이벤트 연결
    connect(clientSocket, &QTcpSocket::readyRead, this, &ClientThread::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ClientThread::onDisconnected);

    exec();  // 스레드 이벤트 루프 실행
}

void ClientThread::setSocket(QTcpSocket* socket){
    clientSocket = socket;
}

void ClientThread::onReadyRead() {
    QByteArray data = clientSocket->readAll();
    qDebug() << "Data received:" << data;
    // 처음 읽는 데이터는 유저 정보로 가정
    if (!isUserAuthenticated) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();

        QString id = jsonObj["ID"].toString();
        QString password = jsonObj["password"].toString();
        QString name = jsonObj["name"].toString();

        if (!id.isEmpty()) {
            // 유저 정보 생성
            user = new USER{id, password, name, clientSocket};
            qDebug() << "User authenticated:" << user->ID;

            // 서버에게 유저 정보 전달
            ChatServer* server = static_cast<ChatServer*>(parent());
            if (server) {
                emit server->AddUser(user);  // 서버에 유저 정보 추가
                server->addClientToMap(this, user);  // clientMap에 추가
            }
            isUserAuthenticated = true;  // 유저 인증 완료
        } else {
            qDebug() << "Invalid user information received";
            clientSocket->disconnectFromHost();  // 연결 종료
        }
    } else {
        // 유저 인증이 완료된 이후는 일반 데이터 처리
        qDebug() << "Data received:" << data;

        // 서버로부터 데이터 처리
        ChatServer* server = static_cast<ChatServer*>(parent());
        if (server) {
            emit server->ProcessData(data, user);
        }
    }
}

void ClientThread::onDisconnected() {
    qDebug() << "Client disconnected: " << user->ID;

    // 서버에게 이 유저를 삭제하도록 요청
    ChatServer* server = static_cast<ChatServer*>(parent());
    if (server) {
        server->removeClient(this);  // 서버에게 클라이언트 삭제 요청
    }

    // 스레드 종료 전 소켓을 안전하게 종료
    if (clientSocket) {
        clientSocket->disconnectFromHost();
        clientSocket->waitForDisconnected();
        clientSocket->deleteLater();
    }
}

void ClientThread::sendImage(const QByteArray& image) {
    if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream out(clientSocket);
        out.setVersion(QDataStream::Qt_5_15);
        int imageSize = image.size();
        out << imageSize;  // 이미지 크기 전송
        out.writeRawData(image.data(), image.size());  // 이미지 데이터 전송
        clientSocket->flush();  // 즉시 전송
    }
}
