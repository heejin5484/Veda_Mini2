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
