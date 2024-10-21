#include "chatserver.h"
#include "mainwindow.h"
#include "constants.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr) {
    mainwindow = static_cast<MainWindow*>(parent);
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket;
    clientSocket->setSocketDescriptor(socketDescriptor);

    QThread *thread = new QThread;
    clientSocket->moveToThread(thread);

    threadList.append(thread); // 스레드 목록에 추가

    connect(thread, &QThread::started, [this, clientSocket, thread]() {
        qDebug() << "새 스레드에서 클라이언트 연결됨";

        connect(clientSocket, &QTcpSocket::readyRead, [=]() {
            QByteArray data = clientSocket->readAll();
            qDebug() << "수신한 사용자 데이터:" << data;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            QJsonObject jsonObj = jsonDoc.object();

            QString type = jsonObj["type"].toString(); // 요청 유형 확인

            if (type == "J") {
                processSignupRequest(jsonObj, clientSocket); // 가입 요청 처리
            } else if (type == "L") {
                processLoginRequest(jsonObj, clientSocket); // 로그인 요청 처리
            } else if (type == "M") {
                processMessageRequest(jsonObj, clientSocket); // 메시지 요청 처리
            } else {
                sendResponse(clientSocket, "error", "잘못된 요청 유형입니다.");
            }
        });

        connect(clientSocket, &QTcpSocket::disconnected, [=]() {
            qDebug() << "클라이언트 연결 해제: " << clientSocket->peerAddress().toString();
            clientMap.remove(clientSocket);

            if (thread->isRunning()) {
                thread->quit();
                thread->wait();
            }
            threadList.removeOne(thread);
            thread->deleteLater();
            qDebug() << "클라이언트 연결 해제 및 스레드 종료";
        });
    });

    thread->start();
    qDebug() << "스레드 초기화";
}

void ChatServer::processSignupRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString userid = jsonObj["userid"].toString();
    QString password = jsonObj["password"].toString();
    QString name = jsonObj["name"].toString();
    QString phone = jsonObj["phone"].toString();
    QString email = jsonObj["email"].toString();

    if (!userid.isEmpty()) {
        USER *user = new USER;
        user->userid = userid;
        user->password = password;
        user->name = name;
        user->usersocket = clientSocket;

        // DatabaseManager 인스턴스 생성 시 유저와 채팅 DB를 모두 전달
        DatabaseManager dbManager("users.db", "chats.db");
        if (!dbManager.init()) {
            sendResponse(clientSocket, "error", "유저 데이터베이스 초기화 실패.");
            return;
        }

        bool saveSuccess = dbManager.saveUserData(name, phone, email, userid, password, "J");
        if (saveSuccess) {
            qDebug() << "사용자 데이터가 성공적으로 저장되었습니다.";
            sendResponse(clientSocket, "success", "가입이 완료되었습니다.");
        } else {
            qDebug() << "사용자 데이터 저장 실패.";
            sendResponse(clientSocket, "error", "가입에 실패했습니다.");
        }

        dbManager.close();
        delete user;

    } else {
        qDebug() << "잘못된 사용자 ID 수신";
        clientSocket->disconnectFromHost();
    }
}

void ChatServer::processLoginRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString userid = jsonObj["userid"].toString();
    QString password = jsonObj["password"].toString();

    qDebug() << "로그인 요청 수신: " << userid;

    DatabaseManager dbManager("users.db", "chats.db");

    if (!dbManager.init()) {
        sendResponse(clientSocket, "error", "유저 데이터베이스 초기화 실패.");
        return;
    }

    QSqlQuery query(dbManager.userDatabase());
    query.prepare("SELECT name, password FROM users WHERE userid = :userid");
    query.bindValue(":userid", userid);

    if (!query.exec()) {
        qDebug() << "쿼리 실행 오류:" << query.lastError().text();
        sendResponse(clientSocket, "error", "쿼리 실행 오류.");
        return;
    }

    if (query.next()) {
        QString storedName = query.value(0).toString();
        QString storedPassword = query.value(1).toString();
        qDebug() << "저장된 이름:" << storedName;
        qDebug() << "저장된 비밀번호:" << storedPassword;

        if (storedPassword == password) {
            USER *user = new USER;
            user->userid = userid;
            user->password = password;
            user->name = storedName;
            user->usersocket = clientSocket;

            QJsonObject responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "로그인 성공";
            responseJson["userid"] = user->userid;
            responseJson["name"] = user->name;
            responseJson["password"] = user->password;
            emit AddUser(user);
            qDebug() << "서버에 추가된 사용자: " << user->userid;
            sendResponse(clientSocket, responseJson);
        } else {
            sendResponse(clientSocket, "fail", "비밀번호가 틀렸습니다.");
        }
    } else {
        sendResponse(clientSocket, "error", "가입되지 않은 사용자입니다.");
    }

    dbManager.close();
}

void ChatServer::processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString message = jsonObj["message"].toString();
    QString userid = jsonObj["userid"].toString();
    QString name = jsonObj["name"].toString();
    qDebug() << "메시지 수신: " << message << " 사용자 ID: " << userid;

    // DatabaseManager 인스턴스 생성 시 유저와 채팅 DB를 모두 전달
    DatabaseManager dbManager("users.db", "chats.db");
    if (!dbManager.init()) {
        sendResponse(clientSocket, "error", "채팅 데이터베이스 초기화 실패.");
        return;
    }

    if (dbManager.saveMessage(name, userid, message)) {
        sendResponse(clientSocket, "success", "메시지 저장 완료.");
    } else {
        sendResponse(clientSocket, "error", "메시지 저장 실패.");
    }

    dbManager.close();
}

void ChatServer::sendResponse(QTcpSocket *clientSocket, const QString &status, const QString &message) const {
    QJsonObject responseJson;
    responseJson["status"] = status; // 예: "success"
    responseJson["message"] = message; // 응답 메시지
    QJsonDocument responseDoc(responseJson);
    QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
    qDebug() << "서버 응답 전송!:" << responseData; // JSON 응답 디버깅 출력
    clientSocket->write(responseData + "\n"); // 개행 문자 추가
    clientSocket->flush();  // 데이터 전송 완료를 보장
}

void ChatServer::sendResponse(QTcpSocket *clientSocket, const QJsonObject &responseJson) const {
    QJsonDocument responseDoc(responseJson);
    QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
    qDebug() << "서버 응답 전송!!:" << responseData; // JSON 응답 디버깅 출력
    clientSocket->write(responseData + "\n"); // 개행 문자 추가
    clientSocket->flush();  // 데이터 전송 완료를 보장
}
