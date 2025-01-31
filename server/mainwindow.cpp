#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "chatserver.h"
#include "chatroom.h"
#include "ui_chatroom.h"
#include "constants.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "rtpprocess.h"


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
    ServerOpen(SERVER_PORT);
    chatroom = new chatRoom(this);
    // 크기 정책 설정
    chatroom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setCentralWidget(chatroom);

    // MainWindow의 최소 크기 설정 (chatroom의 크기 이하로 내려가지않도록
    setMinimumSize(1057, 570);

    // 싱글톤 객체를 통해 FFmpeg 멀티캐스트 스트리밍 시작
    rtpProcess::instance()->startFFmpegProcess();

    connect(this, &MainWindow::NewUserAdd, chatroom, &chatRoom::addUserList);
    connect(this, &MainWindow::DisconnectUser, chatroom, &chatRoom::deleteUserList);
    //connect(server,&ChatServer::messageReceived, chatroom, &chatRoom::onMessageReceived);
}

void MainWindow::ServerOpen(int address) {
    server = new ChatServer(this);

    if (!server->listen(QHostAddress::Any, address)){
        qDebug() << "Server could not start!";
    }
    qDebug() << address << "Opened";
}


void MainWindow::UserConnected(USER* usr){
    qDebug() << "User connected";
    UserMap.insert(usr->userid, usr);
    qDebug() << "Current users in UserMap:" << UserMap.keys(); // 현재 사용자의 ID 로그
    emit NewUserAdd(usr->userid); //user add signal
}

void MainWindow::DataIncome(QByteArray data, USER *user){
    qDebug() << "Data Incoming from user:" << user->userid;

        for (auto otherUser : UserMap) {
            if (otherUser != user) {
                if (otherUser->usersocket) {
                    otherUser->usersocket->write(data);
                    otherUser->usersocket->flush();
                }
            }
        }
}

void MainWindow::UserDisconnected(USER *usr){
    if (UserMap.contains(usr->userid)){
        UserMap.remove(usr->userid);
        qDebug() << "User removed from UserMap: " << usr->userid;
        emit DisconnectUser(usr->userid);
    }

    if (usr->usersocket) {
        delete usr->usersocket;  //
    }
    delete usr;
    qDebug() << "User object deleted";
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // FFmpeg 스트리밍 종료
    rtpProcess::instance()->stopFFmpegProcess();
    QMainWindow::closeEvent(event);
}
