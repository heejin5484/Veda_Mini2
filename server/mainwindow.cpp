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

    // JSON ???Όλ‘? ?°?΄?° ??± (msg ???? ?¬λΆ? ??Έ)
    //QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    //QJsonObject jsonObj = jsonDoc.object();

    //QString type = jsonObj["type"].toString();  // ?°?΄?° ???? ??Έ
    //QString msg = jsonObj["message"].toString();  // λ©μμ§? ?΄?©

    //if (type == "msg") {
        // λ³΄λΈ ?¬?©?λ₯? ? ?Έ? λͺ¨λ  ?¬?©??κ²? λ©μμ§? ? ?¬
        for (auto otherUser : UserMap) {
            if (otherUser != user) {  // λ³΄λΈ ?¬?©? ? ?Έ
                if (otherUser->usersocket) {
                    // ?€λ₯? ?¬?©??κ²? λ©μμ§? ? ?‘
                    otherUser->usersocket->write(data);
                    otherUser->usersocket->flush();  // λ²νΌ? ??Έ ?°?΄?°λ₯? μ¦μ ? ?‘
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
        delete usr->usersocket;  // ?μΌ? λ¨Όμ?? ?­? 
    }
    delete usr;
    qDebug() << "User object deleted";
}
