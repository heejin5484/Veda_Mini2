#include "chatserver.h"
#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr)
{
    mainwindow = static_cast<MainWindow*>(parent);

    // 나중에 스레드풀같은거 생각해보기
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "cccc";
    QTcpSocket *clientSocket = new QTcpSocket;
    clientSocket->setSocketDescriptor(socketDescriptor);

    QThread *thread = new QThread;
    clientSocket->moveToThread(thread); // 소켓을 새 스레드로 이동

    // 스레드 시작되면 데이터 송수신 처리
    connect(thread, &QThread::started, clientSocket, [=](){
        qDebug() << "Client connected in new thread";

        if (clientSocket->waitForReadyRead()){
            QByteArray data = clientSocket->readAll();
            qDebug() << "Client connected:" << data;

            // 데이터를 JSON으로 파싱 ( 나중에 수정 )
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            QJsonObject jsonObj = jsonDoc.object();

            // USER 정보 파싱
            QString id = jsonObj["ID"].toString();
            QString password = jsonObj["password"].toString();
            QString name = jsonObj["name"].toString();

            if (!id.isEmpty()){
                USER *user = new USER{id, "1234", "name", clientSocket};
                emit AddUser(user);

                // 클라이언트 소켓이 준비되었을 때 데이터 읽기
                connect(clientSocket, &QTcpSocket::readyRead, [=](){
                    QByteArray data = clientSocket->readAll();
                    // 수신한 데이터 처리
                    emit ProcessData(data);
                    qDebug() << "Received data:" << data;
                });


                // 클라이언트가 연결 종료시 처리
                connect(clientSocket, &QTcpSocket::disconnected, [=](){
                    qDebug() << "Client disconnected: " << user->ID;

                    emit DisconnectUser(user);

                    clientSocket->deleteLater();
                    // 스레드가 종료되지 않았다면 종료를 기다림
                    if (thread->isRunning()) {
                        thread->quit();
                        thread->wait();  // 스레드가 실행 중일 때만 대기
                    }
                    thread->deleteLater();
                    qDebug() << "Client disconnected and thread finished";
                    // delete는 Main Window에서
                });
            } else {
                qDebug() << "Invalid USER ID received";
                clientSocket->disconnectFromHost();
            }
        }
    });

    // mainwindow와 연결
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected, Qt::UniqueConnection);
    connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);
    connect(this, &ChatServer::DisconnectUser, mainwindow, &MainWindow::UserDisconnected);

    thread->start();
    qDebug() << "init thread";
}
