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
    if (ServerOpen(8080)){
        chatroom = new chatRoom(this, static_cast<ChatServer*>(server));
        setCentralWidget(chatroom);
        connect(this, &MainWindow::NewUserAdd, chatroom, &chatRoom::addUserList);
        connect(this, &MainWindow::DisconnectUser, chatroom, &chatRoom::deleteUserList);
    }
}

bool MainWindow::ServerOpen(int address){
    server = new ChatServer(this);

    if (!server->listen(QHostAddress::Any, address)){
        qDebug() << "Server could not start!";
        return false;
    }
    qDebug() << address << "Opened";
    return true;
}

void MainWindow::UserConnected(USER* usr){
    qDebug() << "User connected";
    UserMap.insert(usr->ID, usr);
    emit NewUserAdd(usr->ID);
}

void MainWindow::DataIncome(QByteArray data, USER *user){
    qDebug() << "Data Incoming from user:" << user->ID;

    //QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    //QJsonObject jsonObj = jsonDoc.object();

    //QString type = jsonObj["type"].toString();
    //QString msg = jsonObj["message"].toString();

    //if (type == "msg") {
        for (auto otherUser : UserMap) {
            if (otherUser != user) {  // send msg except sender
                if (otherUser->usersocket) {
                    otherUser->usersocket->write(data);
                    otherUser->usersocket->flush();
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
        delete usr->usersocket;
    }
    delete usr;
    qDebug() << "User object deleted";
}
