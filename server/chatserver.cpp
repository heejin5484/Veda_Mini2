#include "chatserver.h"

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent) {
    connect(this, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
}

void ChatServer::onNewConnection() {
    QTcpSocket *socket = nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, [socket, this]() {
        QByteArray data = socket->readAll();
        // 데이터 처리
        processMessage(data, socket);
    });

    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void ChatServer::processMessage(const QByteArray &data, QTcpSocket *socket) {

    qDebug() << "Received:" << data;

    // JSON 문자열 파싱
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Invalid JSON received";
        return;
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
    else {
        qDebug() << "Unknown message type";
    }
}

void ChatServer::sendResponse(QTcpSocket *socket, const QJsonObject &response) {
    QJsonDocument doc(response);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write(data + "\n");
    socket->waitForBytesWritten(3000);
}
