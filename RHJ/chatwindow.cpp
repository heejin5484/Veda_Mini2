#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logviewer.h"
#include "logwindow.h"


chatWindow::chatWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatWindow),
    thread(new Thread(this))
{
    ui->setupUi(this);

    // 데이터베이스 초기화
    dbManager = new databaseManager("logs.db");
    if (!dbManager->init()) {
        qDebug() << "Database initialization failed.";
    }

    // 스레드 신호와 슬롯 연결
    connect(thread, &Thread::alert, this, &chatWindow::queueUpdate);
    thread->start(); // 스레드 시작
    connect(ui->logData, &QPushButton::clicked, this, &chatWindow::on_logData_clicked);


}

chatWindow::~chatWindow()
{
    thread->quit(); // 스레드 종료
    thread->wait(); // 스레드 종료 대기
    delete ui;
    delete dbManager;
    delete thread; // 스레드 삭제
}

void chatWindow::on_sendButton_clicked()
{
    QString message = ui->messageline->text();
    ui->textEdit_log->append(message);
    ui->messageline->clear();
}

void chatWindow::saveMessage(const QString &message)
{
    QSqlQuery query;
    query.prepare("INSERT INTO log_messages (message) VALUES (:message)");
    query.bindValue(":message", message);

    if (!query.exec()) {
        qDebug() << "Failed to save message to database: " << query.lastError().text();
    }
}

void chatWindow::queueUpdate()
{
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
}




void chatWindow::on_dbData_clicked()
{
    logViewer *viewer = new logViewer(this);
    viewer->exec();
}

void chatWindow::on_logData_clicked()
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
