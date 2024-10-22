#include "chatserver.h"
#include "mainwindow.h"
#include "constants.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "messagesender.h"

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr) {
    /*mainwindow = static_cast<MainWindow*>(parent);
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
    connect(this, &ChatServer::messageReceived, mainwindow, &MainWindow::DataIncome);
*/
}
void ChatServer::setMainWindow(MainWindow* window)
{
    mainwindow = window;

    // MainWindow가 설정된 후에 연결을 설정합니다.
    if (mainwindow != nullptr) {
        connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
        connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
        connect(this, &ChatServer::messageReceived, mainwindow, &MainWindow::DataIncome);
    } else {
        qDebug() << "MainWindow is nullptr, connections not made.";
    }
    qDebug() << "Current thread: " << QThread::currentThread();
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {

    QTcpSocket *clientSocket = new QTcpSocket;
    if (!clientSocket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error: Could not set socket descriptor";
        delete clientSocket;
        return;
    }

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
        } //else if (type == "M") {
            //processMessageRequest(jsonObj, clientSocket); // 메시지 요청 처리}
        else {
            sendResponse(clientSocket, "error", "잘못된 요청 유형입니다.");
            clientSocket->disconnectFromHost();
        }
    });

    // 클라이언트가 연결을 끊을 때 처리
    connect(clientSocket, &QTcpSocket::disconnected, [=]() {
        qDebug() << "클라이언트 연결 해제: " << clientSocket->peerAddress().toString();
        /*USER* user = clientMap.value(clientSocket, nullptr);
        if (user) {
            qDebug() << "User found:" << user->userid;
            emit DisconnectUser(user);
            clientMap.remove(clientSocket);
        } else {
            // clientSocket에 해당하는 사용자가 없을 경우 처리
            qDebug() << "User not found for the given clientSocket.";
        }
        clientSocket->deleteLater();
        */
    });
}

void ChatServer::processSignupRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString userid = jsonObj["userid"].toString();
    QString password = jsonObj["password"].toString();
    QString name = jsonObj["name"].toString();
    QString phone = jsonObj["phone"].toString();
    QString email = jsonObj["email"].toString();

    if (!userid.isEmpty()) {
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
        //delete user;

    } else {
        qDebug() << "잘못된 사용자 ID 수신";
        clientSocket->disconnectFromHost();
    }
    // 회원가입 성공 후 재로그인 요청
    clientSocket->disconnectFromHost();
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
        clientSocket->disconnectFromHost();
        return;
    }

    if (query.next()) {
        QString storedName = query.value(0).toString();
        QString storedPassword = query.value(1).toString();
        qDebug() << "저장된 이름:" << storedName;
        qDebug() << "저장된 비밀번호:" << storedPassword;

        if (storedPassword == password) {
            // 로그인 성공 시 messageSender 생성
            USER *user = new USER;
            user->userid = userid;
            user->password = password;
            user->name = storedName;
            user->usersocket = clientSocket;

            // 스레드 생성
            QThread* thread = new QThread;
            // messageSender 생성 및 추가
            messageSender* sender = new messageSender();
            sender->moveToThread(thread);
            senderList.insert(sender, user);
            clientSocket->moveToThread(thread);

            QJsonObject responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "로그인 성공";
            responseJson["userid"] = user->userid;
            responseJson["name"] = user->name;
            responseJson["password"] = user->password;
            qDebug() << "서버에 추가된 사용자: " << user->userid;
            sendResponse(clientSocket, responseJson);
            emit AddUser(user);
        } else {
            sendResponse(clientSocket, "fail", "비밀번호가 틀렸습니다.");
            clientSocket->disconnectFromHost();
        }
    } else {
        sendResponse(clientSocket, "error", "가입되지 않은 사용자입니다.");
        clientSocket->disconnectFromHost();
    }

    dbManager.close();
}


/*
void ChatServer::processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString message = jsonObj["message"].toString();
    QString userid = jsonObj["userid"].toString();
    QString name = jsonObj["name"].toString();
    qDebug() << "메시지 수신: " << message << " 사용자 ID: " << userid;
    qDebug() << clientMap.value(clientSocket,nullptr)->userid;
    emit messageReceived(userid, message); // 메시지 방출
    qDebug() << "signal Message received:" << message;
    // DatabaseManager 인스턴스 생성 시 유저와 채팅 DB를 모두 전달
    DatabaseManager dbManager("users.db", "chats.db");
    if (!dbManager.init()) {
        sendResponse(clientSocket, "error", "채팅 데이터베이스 초기화 실패.");
        return;
    }

    if (dbManager.saveMessage(name, userid, message)) {
        sendResponse(clientSocket, "success", "메시지 저장 완료.");
        qDebug() << "emit faill:" << message; // 디버깅 출력
    } else {
        sendResponse(clientSocket, "error", "메시지 저장 실패.");
    }

    dbManager.close();
}
*/

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
