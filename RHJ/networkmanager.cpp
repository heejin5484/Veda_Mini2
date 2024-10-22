#include "networkmanager.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 8889); // 서버 포트 번호

    if (!socket->waitForConnected(3000)) { // 3초 대기
        qDebug() << "서버에 연결할 수 없습니다: " << socket->errorString();
    }
}

void NetworkManager::sendMessage(const QJsonObject &json) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        // JSON 객체를 문자열로 변환
        QJsonDocument doc(json);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        QByteArray data = jsonString.toUtf8();

        // 메시지 전송 (메시지 끝에 newline 추가)
        socket->write(data + "\n");

        // 데이터 전송 완료 대기
        socket->waitForBytesWritten(3000);
    } else {
        qDebug() << "소켓이 연결되지 않았습니다.";
    }
}

void NetworkManager::handleSignupResponse(bool success) {
    emit signupResponse(success);  // 회원가입 응답을 emit(발송)
}
