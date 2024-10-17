#include "videosender.h"

VideoSender::VideoSender() : chatserver(ChatServer::instance())
{
    clientSocket = nullptr;
    isUserAuthenticated = false;
}


VideoSender::~VideoSender() {
        // delete user; 찾아보기 여기서 삭제하는게 적절한지..
}


void VideoSender::setSocket(QTcpSocket* socket){
    clientSocket = socket;

    if (clientSocket == nullptr) {
        qDebug() << "Error: No valid socket passed to thread";
        return;
    }

    qDebug() << "clientSocket is in thread:" << clientSocket->thread();
    qDebug() << "this thread:" << this->thread();
    qDebug() << "Current thread: " << QThread::currentThread();

    connect(clientSocket, &QTcpSocket::readyRead, this, &VideoSender::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &VideoSender::onDisconnected);

    // 서버의 카메라이미지 준비신호와 연결
    connect(&chatserver, &ChatServer::sendImageToClient, this, &VideoSender::sendImage);
}

void VideoSender::onReadyRead() {
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
            emit ChatServer::instance().AddUser(user);  // 서버에 유저 정보 추가
            ChatServer::instance().addClientToMap(this, user);  // clientMap에 추가
            qDebug() << "videosender thread : " << this->thread();
            qDebug() << "cur thread : " << QThread::currentThread();

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

void VideoSender::onDisconnected() {
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


void VideoSender::sendImage(const QByteArray& image) {
    qDebug() << "----------------------------";
    qDebug() << "clientSocket is in thread:" << clientSocket->thread();
    qDebug() << "Current thread:" << QThread::currentThread();
    if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream out(clientSocket);
        out.setVersion(QDataStream::Qt_5_15);
        int imageSize = image.size();
        out << imageSize;  // 이미지 크기 전송
        out.writeRawData(image.data(), imageSize);  // 이미지 데이터 전송
        clientSocket->flush();  // 즉시 전송
        clientSocket->waitForBytesWritten();  // 데이터가 전송될 때까지 대기

        if (clientSocket->error() != QAbstractSocket::UnknownSocketError) {
            qDebug() << "Socket error:" << clientSocket->errorString();
        }
        // 다른 스레드에서 안전하게 실행
    } else {
        qDebug() << "Socket is not connected or invalid";
    }
}
