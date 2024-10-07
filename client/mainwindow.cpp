#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat.h"
#include <QDebug>
#include <QTimer>
#include <QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //나중에 id 입력 구현
    id = "test";

    ui->setupUi(this);
    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred, this, &MainWindow::failedConnect);
    connect(clientSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    ui->status_label->setText("Connecting...");
    tryConnect("127.0.0.1", 54424);
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

    // 2초 대기 후 chat 위젯을 중앙에 배치
    QTimer::singleShot(2000, this, [this]() {
        if (centralWidget()) {
            delete centralWidget();
        }
        ui_chat = new chat(this);
        setCentralWidget(ui_chat);
        connect(ui_chat, &chat::sendMsg_sig,this,&MainWindow::sendMsg);
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
