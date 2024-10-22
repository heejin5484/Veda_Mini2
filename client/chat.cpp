#include "chat.h"
#include "ui_chat.h"
#include "mainwindow.h"
#include <QString>
#include <QDateTime>
#include "rtpclient.h"
chat::chat(NetworkManager *networkManager, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chat)
    , networkManager(networkManager)
{
    ui->setupUi(this);
    // rtpClient의 videoLabel에 ui_chat의 videoLabel 연결
    rtpClient::instance()->videoLabel = ui->video_label;
    // FFmpeg 프로세스 시작
    rtpClient::instance()->startFFmpegProcess();
    qDebug() << "start ffmpeg";
    connect(ui->sendButton,&QPushButton::clicked,this,&chat::onSendButtonClicked);
    connect(qobject_cast<MainWindow*>(this->parent()), &MainWindow::deliverMsg, this, &chat::recieveMsg);
}

chat::~chat()
{
    delete ui;
}

void chat::onSendButtonClicked(){

    QString message = ui->msgLine->text();
    QString dateTime = QDateTime::currentDateTime().toString("HH:mm");

    // 사용자 ID 및 이름을 MainWindow에서 가져옵니다.
    QString userid = qobject_cast<MainWindow*>(this->parent())->currentUser.userid;
    QString name = qobject_cast<MainWindow*>(this->parent())->currentUser.name;

    QString txt = QString("%1 [%2] : %3").arg(qobject_cast<MainWindow*>(this->parent())->id, dateTime, message);
    ui->chatLog->append(txt);
    ui->msgLine->clear();


     // 채팅 메시지를 서버로 보낼 JSON 객체 생성
    QJsonObject json;
    json["type"] = "M";  // 메시지 유형
    json["userid"] = userid;  // 사용자 ID
    json["name"] = name;  // 사용자 이름
    json["message"] = message;  // 채팅 메시지

    networkManager->sendMessage(json);
    // 서버로 JSON 메시지 전송
    //emit sendMsg_sig(json);  // 네트워크 매니저나 해당 클래스로 전송
}

void chat::recieveMsg(QByteArray msg){
    QString txt = QString(msg);
    ui->chatLog->append(msg);
}
