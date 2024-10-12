#include "chatserver.h"
#include "mainwindow.h"

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), mainwindow(nullptr)
{
    mainwindow = static_cast<MainWindow*>(parent);

    // 나중에 스레드풀같은거 생각해보기
}

void ChatServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket;
    clientSocket->setSocketDescriptor(socketDescriptor);

    QThread *thread = new QThread;
    clientSocket->moveToThread(thread); // 소켓을 새 스레드로 이동

    // 스레드 시작되면 데이터 송수신 처리
    connect(thread, &QThread::started, clientSocket, [=](){
        qDebug() << "Client connected in new thread";
        USER user = {"ID1", "1234", "name", clientSocket};
        emit AddUser(user);
    });

    // 클라이언트 소켓이 준비되었을 때 데이터 읽기
    connect(clientSocket, &QTcpSocket::readyRead, [=](){
        QByteArray data = clientSocket->readAll();
        // 수신한 데이터 처리
        emit ProcessData(data);
        qDebug() << "Received data:" << data;
    });

    // 클라이언트가 연결 종료시 처리
    connect(clientSocket, &QTcpSocket::disconnected, [=](){
        clientSocket->deleteLater();
        thread->quit();
        thread->wait();
        thread->deleteLater();
        qDebug() << "Client disconnected and thread finished";
    });

    // mainwindow와 연결
    connect(this, &ChatServer::AddUser, mainwindow, &MainWindow::UserConnected);
    connect(this, &ChatServer::ProcessData, mainwindow, &MainWindow::DataIncome);

    thread->start();
    qDebug() << "init thread";
}
