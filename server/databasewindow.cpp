#include "databasewindow.h"
#include "ui_databasewindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QInputDialog>

DatabaseWindow::DatabaseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseWindow),
    // DatabaseManager를 두 개의 DB와 함께 초기화
    dbManager(new DatabaseManager("users.db", "chats.db")),  // 유저 DB와 채팅 DB 이름을 명시
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
    // 유저 정보를 dbManager에서 불러옴
    QSqlQuery query = dbManager->loadUsers();
    model->setQuery(query);

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
    // 메시지를 dbManager에서 불러옴
    QSqlQuery query = dbManager->loadMessages();
    model->setQuery(query);  // 모델에 쿼리 결과 설정

    if (model->rowCount() == 0) {
        qDebug() << "No messages found.";  // 메시지가 없을 때 로그 출력
    }

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

    // 사용자 이름에 해당하는 메시지를 dbManager에서 가져옴
    QSqlQuery query = dbManager->loadUsers();

    // 쿼리 결과를 모델에 적용
    model->setQuery(query);

    if (model->rowCount() == 0) {
        qDebug() << "No messages found for user " << username;
    }

    // 열 제목 설정
    model->setHeaderData(0, Qt::Horizontal, "메시지");
    model->setHeaderData(1, Qt::Horizontal, "타임스탬프");

    // 테이블 크기 조정
    ui->infoTable->resizeColumnsToContents();
}
