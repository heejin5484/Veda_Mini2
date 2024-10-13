#include "chatserver.h"
#include <QDir>
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
