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

    // JSON ?��?��?���? ?��?��?�� ?��?�� (msg ????�� ?���? ?��?��)
    //QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    //QJsonObject jsonObj = jsonDoc.object();

    //QString type = jsonObj["type"].toString();  // ?��?��?�� ????�� ?��?��
    //QString msg = jsonObj["message"].toString();  // 메시�? ?��?��

    //if (type == "msg") {
        // 보낸 ?��?��?���? ?��?��?�� 모든 ?��?��?��?���? 메시�? ?��?��
        for (auto otherUser : UserMap) {
            if (otherUser != user) {  // 보낸 ?��?��?�� ?��?��
                if (otherUser->usersocket) {
                    // ?���? ?��?��?��?���? 메시�? ?��?��
                    otherUser->usersocket->write(data);
                    otherUser->usersocket->flush();  // 버퍼?�� ?��?�� ?��?��?���? 즉시 ?��?��
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
        delete usr->usersocket;  // ?���? 먼�?? ?��?��
    }
    delete usr;
    qDebug() << "User object deleted";
}
