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
    clientSocket->moveToThread(thread); // 소켓을 새 스레드로 이동

    threadList.append(thread); // 스레드 목록에 추가

    connect(thread, &QThread::started, [this, clientSocket, thread]() {
        qDebug() << "새 스레드에서 클라이언트 연결됨";

        connect(clientSocket, &QTcpSocket::readyRead, [=]() {
            QByteArray data = clientSocket->readAll(); // 수신 데이터 읽기
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

    if (!userid.isEmpty())
    {
        USER *user = new USER; // USER 구조체 인스턴스 생성
        user->userid = userid;
        user->password = password;
        user->name = name;
        user->usersocket = clientSocket;

        //emit AddUser(user); // AddUser 시그널 발생

        DatabaseManager dbManager("users.db"); // 여기서 데이터베이스 매니저를 새로 생성
        if (!dbManager.init()) {
            sendResponse(clientSocket, "error", "데이터베이스 초기화 실패.");
            return; // 초기화 실패 시 응답 후 종료
        }


        bool saveSuccess = dbManager.saveUserData(name, phone, email, userid, password, "J");
        if (saveSuccess) {
            qDebug() << "사용자 데이터가 성공적으로 저장되었습니다.";
            sendResponse(clientSocket, "success", "가입이 완료되었습니다.");
        } else {
            qDebug() << "사용자 데이터 저장 실패.";
            sendResponse(clientSocket, "error", "가입에 실패했습니다.");
        }

        dbManager.close(); // 연결 종료
        delete user;

    }
    else
    {
        qDebug() << "잘못된 사용자 ID 수신";
        clientSocket->disconnectFromHost();
    }


}

void ChatServer::processLoginRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString userid = jsonObj["userid"].toString();
    QString password = jsonObj["password"].toString();

    qDebug() << "로그인 요청 수신: " << userid;

    DatabaseManager dbManager("users.db"); // 데이터베이스 매니저 인스턴스 생성

    if (!dbManager.init()) {
        sendResponse(clientSocket, "error", "데이터베이스 초기화 실패.");
        return; // 초기화 실패 시 응답 후 종료
    }

    // 쿼리 실행
    QSqlQuery query(dbManager.database()); // DatabaseManager에서 db를 가져옵니다.
    query.prepare("SELECT name, password FROM users WHERE userid = :userid"); // 이름도 선택
    query.bindValue(":userid", userid);

    if (!query.exec()) {
        qDebug() << "Query execution error:" << query.lastError().text();
        sendResponse(clientSocket, "error", "쿼리 실행 오류.");
        return; // 쿼리 실행 실패 시 응답 후 종료
    }

    if (query.next()) {
        QString storedName = query.value(0).toString(); // 데이터베이스에서 이름 가져오기
        QString storedPassword = query.value(1).toString(); // 데이터베이스에서 비밀번호 가져오기
        qDebug() << "Stored name:" << storedName;
        qDebug() << "Stored password:" << storedPassword;

        if (storedPassword == password) {
            // 로그인 성공 시 USER 구조체 인스턴스 생성
            USER *user = new USER;
            user->userid = userid; // 사용자 ID 설정
            user->password = password; // 비밀번호 설정
            user->name = storedName; // 사용자 이름 설정
            user->usersocket = clientSocket; // 클라이언트 소켓 설정

            // 클라이언트에게 성공 응답과 함께 이름, ID, 비밀번호 전송
            QJsonObject responseJson;
            responseJson["status"] = "success";
            responseJson["message"] = "로그인 성공";
            responseJson["userid"] = user->userid;
            responseJson["name"] = user->name;
            responseJson["password"] = user->password;
            emit AddUser(user);
            qDebug() << "User added to server: " << user->userid;
            sendResponse(clientSocket, responseJson); // JSON 객체 전송
        }
        else {
            sendResponse(clientSocket, "fail", "비밀번호가 틀렸습니다.");
        }
    }
    else {
        sendResponse(clientSocket, "error", "가입되지 않은 사용자입니다.");
    }

    dbManager.close(); // 연결 종료
}




void ChatServer::processMessageRequest(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString message = jsonObj["message"].toString();
    sendResponse(clientSocket, "success", "메시지 수신 완료");
}

void ChatServer::sendResponse(QTcpSocket *clientSocket, const QString &status, const QString &message) {
    QJsonObject responseJson;
    responseJson["status"] = status; // 예: "success"
    responseJson["message"] = message; // 응답 메시지
    QJsonDocument responseDoc(responseJson);
    QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
    qDebug() << "서버 응답 전송:" << responseData; // JSON 응답 디버깅 출력
    clientSocket->write(responseData + "\n"); // 개행 문자 추가
    clientSocket->flush();  // 데이터 전송 완료를 보장
}

void ChatServer::sendResponse(QTcpSocket *clientSocket, const QJsonObject &responseJson) {
    QJsonDocument responseDoc(responseJson);
    QByteArray responseData = responseDoc.toJson(QJsonDocument::Compact);
    qDebug() << "서버 응답 전송:" << responseData; // JSON 응답 디버깅 출력
    clientSocket->write(responseData + "\n"); // 개행 문자 추가
    clientSocket->flush();  // 데이터 전송 완료를 보장
}

