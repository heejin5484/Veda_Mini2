#include "chatserver.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(static_cast<MainWindow*>(parent)), dbManager(new DatabaseManager("users.db"))
{
    if (!dbManager->init()) {
            qDebug() << "Database initialization failed.";
            return;
        }

    // 나중에 스레드풀같은거 생각해보기

    // mainwindow와 연결
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket;
    clientSocket->setSocketDescriptor(socketDescriptor);

    QThread *thread = new QThread;
    clientSocket->moveToThread(thread); // 소켓을 새 스레드로 이동

    USER *user = new USER{"", "", "", clientSocket};

    threadList.append(thread);
    clientMap.insert(clientSocket, user);

    // 스레드 시작되면 데이터 송수신 처리
    connect(thread, &QThread::started, clientSocket, [=]() {
        qDebug() << "Client connected in new thread";

        if (clientSocket->waitForReadyRead()) {
            QByteArray data = clientSocket->readAll();
            qDebug() << "user data:" << data;

            // 데이터를 JSON으로 파싱 ( 나중에 수정 )
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            QJsonObject jsonObj = jsonDoc.object();

            // USER 정보 파싱
            QString id = jsonObj["ID"].toString();
            user->ID = id;
            QString password = jsonObj["password"].toString();
            QString name = jsonObj["name"].toString();

            if (!id.isEmpty()) {
                emit AddUser(user);

                // 클라이언트 소켓이 준비되었을 때 데이터 읽기
                connect(clientSocket, &QTcpSocket::readyRead, [=]() {
                    QByteArray data = clientSocket->readAll();
                    // 수신한 데이터 처리
                    emit ProcessData(data, user);
                    qDebug() << "Received data:" << data;
                });
            } else {
                qDebug() << "Invalid USER ID received";
                clientSocket->disconnectFromHost();
            }
        }
    });

    // 클라이언트가 연결 종료시 처리
    connect(clientSocket, &QTcpSocket::disconnected, [=]() {
        qDebug() << "Client disconnected: " << user->ID;

        emit DisconnectUser(user);

        // 소켓과 USER 삭제
        clientMap.remove(clientSocket);

        // 메인 스레드에서 스레드 종료를 처리하기 위해 직접 호출
        if (thread->isRunning()) {
            thread->quit();
            thread->wait();  // 스레드가 실행 중일 때만 대기
        }
        threadList.removeOne(thread);  // 스레드 리스트에서 삭제
        thread->deleteLater();

        qDebug() << "Client disconnected and thread finished";
    });

    thread->start();
    qDebug() << "init thread";
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
        if (type == "L") { // 로그인 처리
            QString username = json["username"].toString();
            qDebug() << username << "logged in";

            // DB에서 사용자 확인
            bool loginSuccess = dbManager->checkUser(username, json["password"].toString());
            QJsonObject response;
            response["type"] = "L";
            response["status"] = loginSuccess ? "success" : "failure";
            sendResponse(socket, response);
        }
        else if (type == "O") { // 로그아웃 처리
            QString username = json["username"].toString();
            qDebug() << username << "logged out";

            // 응답 보내기
            QJsonObject response;
            response["type"] = "O";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "J") { // 사용자 가입 처리
            QString username = json["username"].toString();
            QString password = json["password"].toString();
            QString name = json["name"].toString();
            QString phone = json["phone"].toString();
            QString email = json["email"].toString();

            // DB에 사용자 저장
            bool signupSuccess = dbManager->saveUserData(username, password, name, phone, email);
            QJsonObject response;
            response["type"] = "J";
            response["status"] = signupSuccess ? "success" : "failure";
            sendResponse(socket, response);
        }
        else if (type == "I") { // 초대 처리
            QString admin = json["admin"].toString();
            QString target = json["target"].toString();
            qDebug() << admin << "invited" << target;

            // 응답 보내기
            QJsonObject response;
            response["type"] = "I";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "G") { // 철회 처리
            QString admin = json["admin"].toString();
            QString target = json["target"].toString();
            qDebug() << admin << "withdrawal out" << target;

            // 응답 보내기
            QJsonObject response;
            response["type"] = "G";
            response["status"] = "success";
            sendResponse(socket, response);
        }
        else if (type == "F") { // 파일 업로드 처리
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

                        // 파일 저장 성공 응답
                        QJsonObject response;
                        response["type"] = "F";
                        response["status"] = "success";
                        sendResponse(socket, response);
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

// 응답을 클라이언트에게 전송하는 메서드
void ChatServer::sendResponse(QTcpSocket *socket, const QJsonObject &response) {
    QJsonDocument doc(response);
    QByteArray data = doc.toJson();
    socket->write(data + "\n"); // JSON 응답 뒤에 줄바꿈 추가
}
