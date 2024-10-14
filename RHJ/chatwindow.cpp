#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logviewer.h"
#include "logwindow.h"
#include "searchwindow.h"
#include "networkmanager.h"


chatWindow::chatWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatWindow),
    thread(new Thread(this)),
    networkManager(networkManager)
{
    ui->setupUi(this);

    // 데이터베이스 초기화
    databaseManager& dbManager = databaseManager::getInstance();
    if (!dbManager.init()) {
        qDebug() << "Database initialization failed.";
    }

    // 스레드 신호와 슬롯 연결
    connect(thread, &Thread::alert, this, &chatWindow::queueUpdate);
    thread->start(); // 스레드 시작
    connect(ui->logData, &QPushButton::clicked, this, &chatWindow::on_logData_clicked);
    connect(ui->fileSendBtn, &QPushButton::clicked, this, &chatWindow::on_fileSendBtn_clicked);

}

chatWindow::~chatWindow()
{
    thread->quit(); // 스레드 종료
    thread->wait(); // 스레드 종료 대기
    delete ui;
    delete thread; // 스레드 삭제
}

void chatWindow::on_sendButton_clicked()
{
    QString message = ui->messageline->text();
    ui->textEdit_log->append(message);
    ui->messageline->clear();

    // 서버에 메시지 전송
    QJsonObject json;
    json["type"] = "M"; // 메시지 전송 유형
    json["message"] = message;

    networkManager->sendMessage(json); // NetworkManager를 통해 서버에 전송

    // 데이터베이스에 메시지 저장
    if (!dbManager->saveMessage(message)) {
        qDebug() << "Failed to save message to database";
    }

    ui->sendButton->setEnabled(false);

        // 타임아웃을 설정하여 버튼을 다시 활성화 (예: 1초 후)
        QTimer::singleShot(1000, this, [this]() {
            ui->sendButton->setEnabled(true);
        });
}

void chatWindow::on_fileSendBtn_clicked()
{
    qDebug() << "on_fileSendBtn_clicked() called";
    // 파일 선택 다이얼로그 열기
    QString fileName = QFileDialog::getOpenFileName(this, tr("파일 선택"), "", tr("모든 파일 (*.*)"));
    if (fileName.isEmpty()) {
        return; // 사용자가 파일 선택을 취소한 경우
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "파일을 열 수 없습니다: " << file.errorString();
        return;
    }

    // 파일 데이터 읽기
    QByteArray fileData = file.readAll();
    file.close();

    // 서버로 전송할 JSON 객체 생성
    QJsonObject json;
    json["type"] = "F"; // 파일 전송 유형
    json["filename"] = QFileInfo(file).fileName(); // 파일 이름
    //json["data"] = QString::fromUtf8(fileData.toBase64()); // 파일 데이터를 Base64로 인코딩
    json["filePath"] = fileName; // 파일의 전체 경로
    networkManager->sendMessage(json); // NetworkManager를 통해 서버에 전송
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

        //if(!dbManager->saveMessage(msg)) {
            //qDebug() << "Failed to save message to database";
        //}
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

void chatWindow::on_searchBtn_clicked()
{
    SearchWindow *searchWin = new SearchWindow(this);
    searchWin->exec(); // 모달로 실행
    delete searchWin;
}


