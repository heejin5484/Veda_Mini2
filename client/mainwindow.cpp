#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat.h"
#include "networkmanager.h"
#include <QDebug>
#include <QTimer>
#include <QtNetwork>

#define BLOCK_SIZE      1024

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
     ui(new Ui::MainWindow),
     joinWindow(nullptr),
     networkManager(new NetworkManager(this))

{

    ui->setupUi(this);
    clientSocket = new QTcpSocket(this);
    //connect(clientSocket, &QTcpSocket::errorOccurred, this, &MainWindow::failedConnect);
    connect(clientSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(ui->JoinButton, &QPushButton::clicked, this, &MainWindow::on_JoinButton_clicked);
    setConnectButtonEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    // 나중에 로그인 구현
    //id = ui->ID_Edit->text();
    ui->status_label->setText("Connecting...");
    tryConnect("127.0.0.1", 8771);
}

void MainWindow::setConnectButtonEnabled(bool enabled) {
    ui->connectButton->setEnabled(enabled);
}

void MainWindow::tryConnect(QString ip, int port){
    clientSocket->connectToHost(ip, port);
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

    // 연결된 직후 USER 정보를 서버로 전송 (나중에 수정)
    QJsonObject userInfo;
    userInfo["ID"] = id;  // 클라이언트 ID
    userInfo["password"] = "1234";  // 예시 비밀번호
    userInfo["name"] = "TestName";  // 예시 이름

    QJsonDocument doc(userInfo);
    QByteArray userData = doc.toJson(QJsonDocument::Compact);

    // USER 정보를 서버로 전송
    clientSocket->write(userData);
    clientSocket->flush();  // 데이터를 즉시 서버로 전송

    // 2초 대기 후 chat 위젯을 중앙에 배치
    QTimer::singleShot(2000, this, [this]() {
        if (centralWidget()) {
            delete centralWidget();
        }
        ui_chat = new chat(networkManager, this);
        setCentralWidget(ui_chat);
        connect(ui_chat, &chat::sendMsg_sig,this,&MainWindow::sendMsg);
        connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::readMsg);
    });
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
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    emit deliverMsg(bytearray);
}

void MainWindow::on_JoinButton_clicked()
{
    qDebug() << "Join button clicked";
    if (joinWindow == nullptr) {
        NetworkManager *networkManager = new NetworkManager(this);
        joinWindow = new JoinWindow(networkManager, this);
        connect(joinWindow, &JoinWindow::finished, this, [this]() {
            joinWindow = nullptr;
        });
    }
    joinWindow->exec();
}

void MainWindow::on_LoginButton_clicked()
{
    qDebug() << "Login button clicked";

    LoginWindow *loginWindow = new LoginWindow(networkManager, this); // LoginWindow를 힙에 생성
        if (loginWindow->exec() == QDialog::Accepted) {
            // 로그인 성공 처리
            // 여기서 필요에 따라 추가 로직을 구현할 수 있습니다.
        }
    delete loginWindow; // 사용 후 메모리 해제
}

