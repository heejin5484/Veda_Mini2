#include "chat.h"
#include "ui_chat.h"
#include "mainwindow.h"
#include <QString>
#include <QDateTime>

chat::chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chat)
{
    ui->setupUi(this);
    connect(ui->sendButton,&QPushButton::clicked,this,&chat::onSendButtonClicked);
    //connect(qobject_cast<MainWindow*>(this->parent()), &MainWindow::deliverMsg, this, &chat::recieveMsg);
    connect(qobject_cast<MainWindow*>(this->parent()), &MainWindow::deliverMsg, this, &chat::recieveVideo);
}

chat::~chat()
{
    delete ui;
}

void chat::onSendButtonClicked(){

    QString message = ui->msgLine->text();
    QString dateTime = QDateTime::currentDateTime().toString("HH:mm");

    QString txt = QString("%1 [%2] : %3").arg(qobject_cast<MainWindow*>(this->parent())->id, dateTime, message);
    ui->chatLog->append(txt);
    ui->msgLine->clear();

    emit sendMsg_sig(message);
}

void chat::recieveMsg(QByteArray msg){
    QString txt = QString(msg);
    ui->chatLog->append(msg);
}

void chat::recieveVideo(QByteArray msg){
    // QImage로 변환 (JPEG 형식으로 수신)
    QImage image;
    bool loaded = image.loadFromData(msg, "JPEG");  // JPEG 형식으로 이미지 로드
    if (loaded) {
        // QLabel의 크기
        QSize labelSize = ui->videoLabel->size();

        // 이미지의 크기
        QSize imageSize = image.size();

        // 이미지와 QLabel의 종횡비 비교
        QRect targetRect;
        if (static_cast<float>(imageSize.width()) / imageSize.height() > static_cast<float>(labelSize.width()) / labelSize.height()) {
            // 이미지가 가로로 더 길면 좌우를 잘라냄
            int newWidth = static_cast<int>(imageSize.height() * static_cast<float>(labelSize.width()) / labelSize.height());
            int xOffset = (imageSize.width() - newWidth) / 2;
            targetRect = QRect(xOffset, 0, newWidth, imageSize.height());
        } else {
            // 이미지가 세로로 더 길면 위아래를 잘라냄
            int newHeight = static_cast<int>(imageSize.width() * static_cast<float>(labelSize.height()) / labelSize.width());
            int yOffset = (imageSize.height() - newHeight) / 2;
            targetRect = QRect(0, yOffset, imageSize.width(), newHeight);
        }

        // 이미지를 잘라냄
        QImage croppedImage = image.copy(targetRect);

        // 잘린 이미지를 QLabel 크기에 맞게 설정
        QPixmap pixmap = QPixmap::fromImage(croppedImage).scaled(labelSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        ui->videoLabel->setPixmap(pixmap);
    } else {
        qDebug() << "Failed to load image from received data.";
    }
}
