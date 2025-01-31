#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logwindow.h"
#include "logviewer.h"
#include "thread.h"
#include "loginwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QQueue>
#include <QTextEdit>
#include <QSqlRecord>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    thread = new Thread(nullptr);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);


    // dbManager init
    dbManager = new databaseManager("logs.db");
    if (!dbManager->init()) {
        qDebug() << "Database initialization failed.";
    }

    // slot connect
    //connect(ui->logDataButton, &QPushButton::clicked, this, &MainWindow::on_LogData_clicked);
    connect(thread, SIGNAL(alert()), SLOT(queueUpdate()));


    thread->start();
}

MainWindow::~MainWindow()
{
    thread->quit(); // 스레드 종료
    thread->wait();
    delete ui;
    delete dbManager;
    //thread->terminate();
}

void MainWindow::on_sendButton_clicked()
{
    QString message = ui->messageline->text();
    //QString dateTime = QDateTime::currentDateTime().toString("HH:mm");

    // QString txt = QString("%1 [%2] : %3").arg(ui->idLabel->text(), dateTime, message);
    ui->textEdit_log->append(message);

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

    if (!msgqueue.isEmpty()) {
        QString msg = msgqueue.dequeue();
        ui->textEdit_queue->append(msg);

        if(!dbManager->saveMessage(msg)) {
            qDebug() << "Failed to save message to database";
        }
    }

    // 업데이트 디버깅용
    //ui->textEdit_queue->append(msgqueue.dequeue());
}

void MainWindow::on_LogData_clicked()
{
    QSqlQuery query;
    QString logData;

    if (query.exec("SELECT timestamp, message FROM log_messages ORDER BY id ASC")) {
        while (query.next()) {
            QString timestamp = query.value(0).toString();
            QString message = query.value(1).toString();
            logData += QString("[%1] %2\n").arg(timestamp, message);
        }
    } else {
        qDebug() << "Failed to retrieve data from database: " << query.lastError().text();
        logData = "Failed to load log data from database.";
    }

    logWindow *logWindowInstance = new logWindow(this);
    logWindowInstance->setLogData(logData);
    logWindowInstance->exec();
}

void MainWindow::on_pushButton_clicked() // db
{
    logViewer *viewer = new logViewer(this);
    viewer->exec();
}


void MainWindow::on_logDataButton_clicked() // log
{
    logWindow *logWindowInstance = new logWindow(this);
    logWindowInstance->show();
}

void MainWindow::on_idButton_1_clicked()
{
    //ui->idLabel->setText("id1");
}


void MainWindow::on_idButton_2_clicked()
{
    //ui->idLabel->setText("id2");
}


void MainWindow::on_idButton_3_clicked()
{
    //ui->idLabel->setText("id3");
}


void MainWindow::on_startButton_clicked()
{
    NetworkManager *networkManager = new NetworkManager;
    loginWindow *loginWindowInstance = new loginWindow(networkManager, this);  // 변수 이름을 loginWindowInstance로 변경
    loginWindowInstance->show();
    this->hide();
}
