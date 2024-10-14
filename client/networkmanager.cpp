#include "networkmanager.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 8899); // 서버 포트 번호

    if (!socket->waitForConnected(3000)) {  // 3초 대기
        qDebug() << "서버에 연결할 수 없습니다: " << socket->errorString();
    } else {
        connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    }
}

void NetworkManager::sendMessage(const QJsonObject &json) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        QJsonDocument doc(json);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        QByteArray data = jsonString.toUtf8();
        socket->write(data + "\n"); // 메시지 전송

        if (!socket->waitForBytesWritten(3000)) {
            qDebug() << "데이터 전송 중 오류 발생: " << socket->errorString();
        }
    } else {
        qDebug() << "소켓이 연결되지 않았습니다.";
    }
}

void NetworkManager::onReadyRead() {
    while (socket->canReadLine()) { // 여러 줄의 응답 처리
        QByteArray responseData = socket->readLine();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            QJsonObject jsonObject = jsonResponse.object();
            // Check for the specific response format based on your server implementation
            if (jsonObject.contains("type") && jsonObject["type"].toString() == "J") { // Checking the type
                bool success = jsonObject["status"].toString() == "success"; // Update according to your server's response
                QString message = jsonObject.contains("message") ? jsonObject["message"].toString() : "서버에서 응답이 없습니다.";
                emit signupResponse(success, message);
            } else {
                qDebug() << "서버 응답에 type 필드가 없습니다.";
            }
        } else {
            qDebug() << "서버 응답을 JSON으로 파싱하지 못했습니다.";
        }
    }
}
