#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QQueue>
#include <QTextEdit>
#include "thread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    thread = new Thread(nullptr);

    connect(thread, SIGNAL(alert()), SLOT(queueUpdate()));
    thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    thread->terminate();
}

void MainWindow::on_sendButton_clicked()
{
    QString message = ui->messageline->text();
    QString dateTime = QDateTime::currentDateTime().toString("HH:mm");

    QString txt = QString("%1 [%2] : %3").arg(ui->idLabel->text(), dateTime, message);
    ui->textEdit_log->append(txt);
    ui->messageline->clear();
}

void MainWindow::queueUpdate(){
    QTextEdit* log = ui->textEdit_log;
    QString fullText = log->toPlainText(); // 전체 텍스트 가져옴

    if (lastCopied < fullText.length()){
        if (fullText[lastCopied] == '\n') lastCopied++;
        QString newText = fullText.mid(lastCopied);  // 마지막 업데이트 이후 위치에서부터
        msgqueue.enqueue(newText);  // 큐에 추가
        lastCopied = fullText.length();  // 인덱스 업데이트
    }

    // 업데이트 디버깅용
    ui->textEdit_queue->append(msgqueue.dequeue());
}

void MainWindow::on_idButton_1_clicked()
{
    ui->idLabel->setText("id1");
}


void MainWindow::on_idButton_2_clicked()
{
    ui->idLabel->setText("id2");
}


void MainWindow::on_idButton_3_clicked()
{
    ui->idLabel->setText("id3");
}

