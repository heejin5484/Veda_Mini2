#include "messagesender.h"

messageSender::messageSender() : chatServer(ChatServer::instance())
{
    clientSocket = nullptr;
}

messageSender::~messageSender(){
    qDebug()<<"goodbye";
    qDebug()<<user->userid;
}

void messageSender::setSocket(QTcpSocket* socket){
    clientSocket = socket;

    if (clientSocket == nullptr) {
        qDebug() << "Error: No valid socket passed to thread";
        return;
    }

    qDebug() << "clientSocket is in thread:" << clientSocket->thread();
    qDebug() << "this thread:" << this->thread();
    qDebug() << "Current thread: " << QThread::currentThread();

    connect(clientSocket, &QTcpSocket::readyRead, this, &messageSender::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &messageSender::onDisconnected);

    connect(&chatServer, &ChatServer::sendMessageToClient, this, &messageSender::sendMessage);
}


void messageSender::onReadyRead() {
    QByteArray data = clientSocket->readAll();
    qDebug() << "Data received:" << data;

    qDebug() << "Data received:" << data;

    // 서버로부터 데이터 처리
    //emit ChatServer::instance().ProcessData(data, user);

}


void messageSender::onDisconnected() {
    qDebug() << "Client disconnected: " << user->userid;

    // 서버에게 이 유저를 삭제하도록 요청
    //ChatServer::instance().removeClient(this);  // 서버에게 클라이언트 삭제 요청

    // 스레드 종료 전 소켓을 안전하게 종료 ->> 삭제로 바꿔야할듯
    if (clientSocket) {
        qDebug() << "Client disconnected, deleting socket.";
        clientSocket->deleteLater();  // 안전하게 삭제
        clientSocket = nullptr;  // 포인터를 nullptr로 설정하여 재사용 방지
    }
    //emit disconnect();
}


void messageSender::sendMessage(const QString msg) {
    if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "----------------------------";
        qDebug() << "clientSocket is in thread:" << clientSocket->thread();
        qDebug() << "Current thread:" << QThread::currentThread();
        /*QDataStream out(clientSocket);
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
*/
    }
}
