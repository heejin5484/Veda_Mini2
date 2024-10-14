#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "chatserver.h"
#include "chatroom.h"
#include "ui_chatroom.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Login_button_clicked()
{
    LoginSuccess();
}

void MainWindow::LoginSuccess(){
    ServerOpen(8080);
    chatroom = new chatRoom(this);
    setCentralWidget(chatroom);
    connect(this, &MainWindow::NewUserAdd, chatroom, &chatRoom::addUserList);
    connect(this, &MainWindow::DisconnectUser, chatroom, &chatRoom::deleteUserList);
}

void MainWindow::ServerOpen(int address){
    server = new ChatServer(this);

    if (!server->listen(QHostAddress::Any, address)){
        qDebug() << "Server could not start!";
    }
    qDebug() << address << "Opened";
}

void MainWindow::UserConnected(USER* usr){
    qDebug() << "User connected";
    UserMap.insert(usr->ID, usr);
    emit NewUserAdd(usr->ID);
}

void MainWindow::DataIncome(QByteArray data, USER *user){
    qDebug() << "Data Incoming from user:" << user->ID;

    // JSON 형식으로 데이터 파싱 (msg 타입 여부 확인)
    //QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    //QJsonObject jsonObj = jsonDoc.object();

    //QString type = jsonObj["type"].toString();  // 데이터 타입 확인
    //QString msg = jsonObj["message"].toString();  // 메시지 내용

    //if (type == "msg") {
        // 보낸 사용자를 제외한 모든 사용자에게 메시지 전달
        for (auto otherUser : UserMap) {
            if (otherUser != user) {  // 보낸 사용자 제외
                if (otherUser->usersocket) {
                    // 다른 사용자에게 메시지 전송
                    otherUser->usersocket->write(data);
                    otherUser->usersocket->flush();  // 버퍼에 쌓인 데이터를 즉시 전송
                }
            }
        }
    //} else {
    //    qDebug() << "Unhandled data type: " << type;
    //}

}

void MainWindow::UserDisconnected(USER *usr){
    if (UserMap.contains(usr->ID)){
        UserMap.remove(usr->ID);
        qDebug() << "User removed from UserMap: " << usr->ID;
        emit DisconnectUser(usr->ID);
    }

    if (usr->usersocket) {
        delete usr->usersocket;  // 소켓 먼저 삭제
    }
    delete usr;
    qDebug() << "User object deleted";
}
