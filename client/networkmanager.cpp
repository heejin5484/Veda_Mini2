#include "networkmanager.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 8771); // 서버 포트 번호

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

        // 전송할 메시지 출력
        qDebug() << "보낼 메시지: " << jsonString;

        // 메시지 전송
        qint64 bytesWritten = socket->write(data + "\n");

        if (bytesWritten == -1) {
            qDebug() << "데이터 전송 중 오류 발생: " << socket->errorString();
        } else {
            // 성공적으로 전송된 바이트 수를 출력
            qDebug() << "성공적으로 보낸 바이트 수: " << bytesWritten;
        }

        // 전송 완료 대기 (선택 사항)
        if (!socket->waitForBytesWritten(3000)) {
            qDebug() << "데이터 전송 중 오류 발생(타임아웃): " << socket->errorString();
        }
    } else {
        qDebug() << "소켓이 연결되지 않았습니다.";
    }
}


void NetworkManager::onReadyRead() {
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine();
        qDebug() << "수신한 응답:" << line; // 응답 내용을 로그로 확인

        QJsonDocument jsonDoc = QJsonDocument::fromJson(line);
        if (jsonDoc.isNull()) {
            qDebug() << "서버 응답을 JSON으로 파싱하지 못했습니다:" << line; // 에러 메시지 추가
            return; // JSON 파싱 실패 시 함수 종료
        }

        QJsonObject jsonObj = jsonDoc.object();
        bool success = jsonObj["status"] == "success";
        QString message = jsonObj["message"].toString();

        emit signupResponse(success, message); // 회원가입 응답 시그널 전송
    }
}
