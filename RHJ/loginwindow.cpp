#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "chatwindow.h"
#include "networkmanager.h"

loginWindow::loginWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginWindow),
    chatWindoww(nullptr),
    networkManager(networkManager)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &loginWindow::on_loginButton_clicked);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &loginWindow::on_logoutBtn_clicked);
    connect(ui->joinBtn, &QPushButton::clicked, this, &loginWindow::on_joinBtn_clicked);
    connect(ui->withdrawalBtn, &QPushButton::clicked, this, &loginWindow::on_withdrawalBtn_clicked);
    connect(ui->inviteBtn, &QPushButton::clicked, this, &loginWindow::on_inviteBtn_clicked);
}

loginWindow::~loginWindow()
{
    delete ui;
    delete chatWindoww;
}

void loginWindow::on_loginButton_clicked()
{
    //QJsonObject json;
    //json["type"] = "L"; // 로그인 유형
    //json["username"] = "sy"; // 사용자 이름
    //json["password"] = "4567"; // 비밀번호

    //networkManager->sendMessage(json);  // JSON 객체를 서버에 전송

    if (chatWindoww == nullptr) {
            chatWindoww = new chatWindow(networkManager);  // chatWindow 객체 생성
        }

    chatWindoww->show();
    this->hide();
}

void loginWindow::on_logoutBtn_clicked()
{
    QString username = "sy";

    QJsonObject json;
    json["type"] = "O"; // 로그아웃 유형
    json["username"] = username;

    networkManager->sendMessage(json);  // JSON 객체를 서버에 전송

    QTimer::singleShot(1000, this, [this]() {
            ui->logoutBtn->setEnabled(true);
        });
}

void loginWindow::on_joinBtn_clicked()
{
    QJsonObject json;
    json["type"] = "J"; // 회원가입 유형
    json["username"] = "sy"; // 사용자 이름
    json["password"] = "1234"; // 비밀번호

    networkManager->sendMessage(json);  // JSON 객체를 서버에 전송
}

void loginWindow::on_inviteBtn_clicked()
{
    QJsonObject json;
    json["type"] = "I"; // 초대 유형
    json["admin"] = "se"; // 관리자 ID
    json["target"] = "hj"; // 초대할 사용자

    networkManager->sendMessage(json);  // JSON 객체를 서버에 전송
}

void loginWindow::on_withdrawalBtn_clicked()
{
    QJsonObject json;
    json["type"] = "G"; // 강퇴 유형
    json["admin"] = "sy"; // 관리자 ID
    json["target"] = "hj"; // 강퇴할 사용자

    networkManager->sendMessage(json);  // JSON 객체를 서버에 전송
}



