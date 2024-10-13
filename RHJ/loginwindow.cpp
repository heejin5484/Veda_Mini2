#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "chatwindow.h"


loginWindow::loginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginWindow),
    chatWindoww(nullptr)
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

    //sendMessageToServer(json);  // JSON 객체를 서버에 전송

    if (chatWindoww == nullptr) {
            chatWindoww = new chatWindow();  // chatWindow 객체 생성
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

    sendMessageToServer(json);  // JSON 객체를 서버에 전송
}

void loginWindow::on_joinBtn_clicked()
{
    QJsonObject json;
    json["type"] = "J"; // 회원가입 유형
    json["username"] = "sy"; // 사용자 이름
    json["password"] = "1234"; // 비밀번호

    sendMessageToServer(json);  // JSON 객체를 서버에 전송
}

void loginWindow::on_inviteBtn_clicked()
{
    QJsonObject json;
    json["type"] = "I"; // 초대 유형
    json["admin"] = "se"; // 관리자 ID
    json["target"] = "hj"; // 초대할 사용자

    sendMessageToServer(json);  // JSON 객체를 서버에 전송
}

void loginWindow::on_withdrawalBtn_clicked()
{
    QJsonObject json;
    json["type"] = "G"; // 강퇴 유형
    json["admin"] = "sy"; // 관리자 ID
    json["target"] = "hj"; // 강퇴할 사용자

    sendMessageToServer(json);  // JSON 객체를 서버에 전송
}

// JSON 객체를 문자열로 변환하여 서버에 전송하는 함수
void loginWindow::sendMessageToServer(const QJsonObject &json) {


    QTcpSocket *socket = new QTcpSocket(this);

    socket->connectToHost(QHostAddress::LocalHost, 8888); // 서버 포트 번호

    if (socket->waitForConnected(3000)) { // 3초 동안 대기
        // JSON 객체를 문자열로 변환
        QJsonDocument doc(json);
        QString jsonString = doc.toJson(QJsonDocument::Compact);

        // 문자열을 바이트 배열로 변환
        QByteArray data = jsonString.toUtf8();

        // 메시지 전송 (메시지 끝에 newline 추가)
        socket->write(data + "\n");

        // 데이터 전송 완료 대기
        socket->waitForBytesWritten(3000);

        socket->close();
    } else {
        qDebug() << "서버에 연결할 수 없습니다: " << socket->errorString();
    }
}


