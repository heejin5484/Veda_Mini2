#include "databasewindow.h"
#include "ui_databasewindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QInputDialog>

DatabaseWindow::DatabaseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseWindow),
    dbManager(new DatabaseManager("users.db")), // 데이터베이스 초기화
    model(new QSqlQueryModel(this))  // 모델 초기화
{
    ui->setupUi(this);

    // 데이터베이스 초기화
    if (!dbManager->init()) {
        qDebug() << "Database initialization failed!";
        return;
    }

    // 필터 항목 설정
    ui->filterComboBox->addItem("사용자 정보");
    ui->filterComboBox->addItem("메시지 정보");
    ui->filterComboBox->addItem("특정 사용자 메시지");

    // 필터 변경 시 연결
    connect(ui->filterComboBox, &QComboBox::currentTextChanged, this, &DatabaseWindow::onFilterChanged);

    // 테이블 모델 설정
    ui->infoTable->setModel(model);
}

DatabaseWindow::~DatabaseWindow()
{
    delete dbManager;  // 메모리 해제
    delete ui;
}

void DatabaseWindow::onFilterChanged(const QString &filter) {
    if (filter == "사용자 정보") {
        loadUsers();  // 사용자 정보를 불러옴
    } else if (filter == "메시지 정보") {
        loadMessages();  // 메시지 정보를 불러옴
    } else if (filter == "특정 사용자 메시지") {
        loadMessagesByUser();  // 특정 사용자의 메시지를 불러옴
    }
}

void DatabaseWindow::loadUsers() {
    QSqlQuery query = dbManager->loadUsers();  // 사용자 정보 쿼리 가져오기
    model->setQuery(query);  // 모델에 쿼리 결과 설정

    if (model->rowCount() == 0) {
        qDebug() << "No data found.";  // 데이터가 없을 경우 로그 출력
    }

    // 열 제목 설정
    model->setHeaderData(0, Qt::Horizontal, "이름");
    model->setHeaderData(1, Qt::Horizontal, "전화번호");
    model->setHeaderData(2, Qt::Horizontal, "이메일");
    model->setHeaderData(3, Qt::Horizontal, "사용자 이름");
    model->setHeaderData(4, Qt::Horizontal, "비밀번호");

    // 테이블 크기 조정
    ui->infoTable->resizeColumnsToContents();
}



void DatabaseWindow::loadMessages() {
    if (!dbManager->openDatabase()) { // 데이터베이스 열기 시도
        qDebug() << "Database could not be opened.";
        return;
    }

    // 메시지와 사용자 정보를 조인하여 가져옴
    QSqlQuery query(dbManager->database()); // 올바른 db 객체 사용
    query.prepare("SELECT log_messages.message, users.userid, log_messages.timestamp "
                   "FROM log_messages "
                   "JOIN users ON log_messages.userid = users.userid");
    qDebug() << "Row count 1 :" << model->rowCount();  // 데이터가 몇 개인지 확인

    if (!query.exec()) {
        qDebug() << "Failed to load messages: " << query.lastError().text();
        qDebug() << "Query: " << query.lastQuery();  // 마지막 실행된 쿼리 확인
        return;  // 오류 발생 시 함수 종료
    }

    qDebug() << "Row count 2 :" << model->rowCount();  // 데이터가 몇 개인지 확인
    if (model->rowCount() > 0) {
        // 데이터가 있을 때만 fetch 수행
        if (query.first()) {
            do {
                qDebug() << "Fetched message:" << query.value(0).toString()
                         << ", User ID:" << query.value(1).toString()
                         << ", Timestamp:" << query.value(2).toString();
            } while (query.next());
        }
    }


    // 쿼리 결과가 있는지 확인
    if (!query.first()) {
        qDebug() << "No messages found.";  // 메시지가 없을 때
        return;
    }






    // 모델에 쿼리 세팅
    model->setQuery(query);

    // 열 제목 설정
    model->setHeaderData(0, Qt::Horizontal, "메시지");
    model->setHeaderData(1, Qt::Horizontal, "사용자 이름");
    model->setHeaderData(2, Qt::Horizontal, "타임스탬프");

    // 테이블 크기 조정
    ui->infoTable->resizeColumnsToContents();
}




void DatabaseWindow::loadMessagesByUser() {
    // 사용자 이름을 입력 받음
    QString username = QInputDialog::getText(this, "사용자 이름 입력", "특정 사용자의 메시지를 보려면 사용자 이름을 입력하세요:");

    // 입력된 사용자 이름이 비어있지 않을 때만 실행
    if (username.isEmpty()) return;

    QSqlQuery query;
    // 입력한 사용자 이름에 해당하는 메시지를 가져옴
    query.prepare("SELECT messages.message, messages.timestamp FROM messages JOIN users ON messages.user_id = users.id WHERE users.userid = :username");
    query.bindValue(":username", username);

    // 쿼리 실행
    if (!query.exec()) {
        qDebug() << "Query error: " << query.lastError().text();  // 에러가 발생하면 출력
    } else {
        model->setQuery(query);  // 모델에 쿼리 결과 설정
        model->setHeaderData(0, Qt::Horizontal, "메시지");
        model->setHeaderData(1, Qt::Horizontal, "타임스탬프");

        ui->infoTable->resizeColumnsToContents();  // 테이블 크기 조정
    }
}
