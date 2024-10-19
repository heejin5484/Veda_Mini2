#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat.h"
#include <QDebug>
#include <QTimer>
#include <QtNetwork>

#define BLOCK_SIZE      1024

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    clientSocket = new QTcpSocket(this);
    videoSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred, this, &MainWindow::failedConnect);
    connect(clientSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(videoSocket, &QTcpSocket::connected, this, &MainWindow::onConnected_vid);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    // 나중에 로그인 구현
    id = ui->ID_Edit->text();
    ui->status_label->setText("Connecting...");
    tryConnect("127.0.0.1", 8080);
}

void MainWindow::tryConnect(QString ip, int port){
    // 메세지용 소켓 연결
    clientSocket->connectToHost(ip, port);
    // 비디오 전송용 소켓 연결
    videoSocket->connectToHost(ip, port+1);  // 비디오 전송을 위한 포트
}

void MainWindow::failedConnect()
{
    ui->status_label->setText("Connecting Failed.");
    qDebug( ) << clientSocket->errorString();
    // 2초 대기 후 원래대로 되돌림
    QTimer::singleShot(2000, this, [this]() {
        ui->status_label->setText("Welcome!");
    });
}

void MainWindow::onConnected()
{
    ui->status_label->setText("Connecting succeeded!");

    // 연결된 직후 소켓 용도를 서버로 전송 (나중에 수정)
    QJsonObject jsonObj;
    jsonObj["type"] = "message";  // 메시지 소켓임을 알림
    jsonObj["ID"] = id;  // 클라이언트 ID
    jsonObj["password"] = "1234";  // 예시 비밀번호
    jsonObj["name"] = "TestName";  // 예시 이름
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);


    // USER 정보를 서버로 전송
    clientSocket->write(jsonData);
    clientSocket->flush();  // 데이터를 즉시 서버로 전송


    // 2초 대기 후 chat 위젯을 중앙에 배치
    QTimer::singleShot(2000, this, [this]() {
        if (centralWidget()) {
            delete centralWidget();
        }
        ui_chat = new chat(this);
        setCentralWidget(ui_chat);
        connect(ui_chat, &chat::sendMsg_sig,this,&MainWindow::sendMsg);
        //connect(clientSocket, SIGNAL(readyRead( )), SLOT(readMsg()));

        connect(ui_chat, &chat::sendMsg_sig,this,&MainWindow::sendMsg);
        //connect(videoSocket, SIGNAL(readyRead( )), SLOT(readMsg()));
    });
}

void MainWindow::onConnected_vid()
{    // 비디오 소켓 연결
    QJsonObject jsonObj;
    jsonObj["type"] = "video";  // 메시지 소켓임을 알림
    jsonObj["ID"] = id;  // 클라이언트 ID
    jsonObj["password"] = "1234";  // 예시 비밀번호
    jsonObj["name"] = "TestName";  // 예시 이름
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    videoSocket->write(jsonData);
    videoSocket->flush();
    connect(videoSocket, SIGNAL(readyRead( )), SLOT(readMsg()));
}

void MainWindow::sendMsg(QString msg)
{
    qDebug() << "msg";
    if(msg.length()) {
        QByteArray bytearray;
        bytearray = msg.toUtf8();
        clientSocket->write(bytearray);
    }
}

void MainWindow::readMsg()
{
    // 나중에 여기 split해서 파싱한 문자에따라 다르게 동작하게끔 switch문/signal 구현
    //QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    //emit deliverMsg(bytearray);
    static QByteArray buffer;
    static int expectedSize = -1;

    QDataStream in(videoSocket);
    in.setVersion(QDataStream::Qt_5_15);

    if (expectedSize == -1) {
        // 이미지 크기를 먼저 수신
        if (videoSocket->bytesAvailable() < sizeof(int)) {
            return;  // 이미지 크기를 받을 만큼 데이터가 없으면 리턴
        }
        in >> expectedSize;  // 이미지 크기 수신
    }

    // 이미지 데이터를 수신
    if (videoSocket->bytesAvailable() < expectedSize) {
        return;  // 모든 데이터를 받을 때까지 대기
    }

    buffer.append(videoSocket->read(expectedSize));  // 데이터 읽기

    if (buffer.size() == expectedSize) {
        // 이미지가 완전히 수신된 경우
        emit deliverMsg(buffer);  // 완성된 이미지를 처리하도록 신호 전송

        // 초기화
        expectedSize = -1;
        buffer.clear();
    }
}
