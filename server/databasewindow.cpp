#include "databasewindow.h"
#include "ui_databasewindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

DatabaseWindow::DatabaseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseWindow),

    dbManager(new DatabaseManager("users.db", "chats.db")),  // 유저 DB와 채팅 DB 이름을 명시
    model(new QSqlQueryModel(this))  // 모델 초기화
{
    ui->setupUi(this);

    ui->filterComboBox->setStyleSheet("QComboBox { min-width: 300px; }");
    ui->filterComboBox->setStyleSheet(
        "QComboBox {"
        "   background-color: white;"
        "   color: black;"
        "   border: 1px solid purple;"
        "}"
        "QComboBox::drop-down {"
        "   background-color: purple;"
        "   color: white;"
        "   border: none;"
        "}"
        "QComboBox::item {"
        "   background-color: white;"
        "   color: black;"
        "}"
        "QComboBox::item:selected {"
        "   background-color: purple;"
        "   color: white;"
        "}"
    );

    this->setStyleSheet("background-color: #f0f0f0;");

    connect(ui->refreshButton, &QPushButton::clicked, this, &DatabaseWindow::refreshData);
    if (!dbManager->init()) {
        qDebug() << "Database initialization failed!";
        return;
    }


    // Table styles
    ui->infoTable->setStyleSheet("QWidget { background-color: white; }"
                                 "QPushButton { background-color: purple; color: white; border: none; }"
                                 "QPushButton:hover { background-color: darkviolet; }"
                                 "QTableView { border: 1px solid purple; }"
                                 "QHeaderView::section { background-color: purple; color: white; }");

    ui->filterComboBox->addItem("사용자 정보");
    ui->filterComboBox->addItem("메시지 정보");
    ui->filterComboBox->addItem("특정 사용자 메시지");
    ui->filterComboBox->addItem("메시지 검색");

    connect(ui->filterComboBox, &QComboBox::currentTextChanged, this, &DatabaseWindow::onFilterChanged);
    ui->infoTable->setModel(model);
}

DatabaseWindow::~DatabaseWindow()
{
    delete dbManager;
    delete ui;
}

void DatabaseWindow::refreshData() {

    QString currentFilter = ui->filterComboBox->currentText();

        // 기본 항목에 맞는 데이터를 로드
        if (currentFilter == "사용자 정보") {
            loadUsers(); // 사용자 정보를 로드
        } else if (currentFilter == "메시지 정보") {
            loadMessages(); // 메시지 정보를 로드
        } else if (currentFilter == "특정 사용자 메시지") {
            loadMessagesByUser(); // 특정 사용자 메시지를 로드
        } else if (currentFilter == "메시지 검색") {
            loadMessagesByKeyword(); // 메시지 검색을 로드
        }
}

void DatabaseWindow::onFilterChanged(const QString &filter) {
    if (filter == "사용자 정보") {
        loadUsers();
    } else if (filter == "메시지 정보") {
        loadMessages();
    } else if (filter == "특정 사용자 메시지") {
        loadMessagesByUser();
    } else if (filter == "메시지 검색") {
        loadMessagesByKeyword();
    }
}

void DatabaseWindow::loadUsers() {

    model->clear();
    QSqlQuery query = dbManager->loadUsers();
    model->setQuery(query);

    if (model->rowCount() == 0) {
        qDebug() << "No data found.";
    }

    model->setHeaderData(0, Qt::Horizontal, "Name");
    model->setHeaderData(1, Qt::Horizontal, "Phone");
    model->setHeaderData(2, Qt::Horizontal, "E-mail");
    model->setHeaderData(3, Qt::Horizontal, "ID");
    model->setHeaderData(4, Qt::Horizontal, "PW");


    ui->infoTable->resizeColumnsToContents();
}

void DatabaseWindow::loadMessages() {

    model->clear();
    QSqlQuery query = dbManager->loadMessages();
    model->setQuery(query);

    if (model->rowCount() == 0) {
        qDebug() << "No messages found.";
    }

    model->setHeaderData(0, Qt::Horizontal, "Name");
    model->setHeaderData(1, Qt::Horizontal, "ID");
    model->setHeaderData(2, Qt::Horizontal, "Message");
    model->setHeaderData(3, Qt::Horizontal, "Time");

    ui->infoTable->resizeColumnsToContents();
}

void DatabaseWindow::loadMessagesByUser() {
    QString input = QInputDialog::getText(this, "사용자 이름 또는 ID 입력", "특정 사용자의 메시지를 보려면 이름이나 사용자 ID를 입력하세요:");
    if (input.isEmpty()) return;

    model->clear();

    QSqlQuery query(dbManager->chatDatabase());
    query.prepare("SELECT name, userid, message, timestamp FROM chats WHERE userid = :input OR name = :input"); // Filter by userid or name
    query.bindValue(":input", input); // Bind the input parameter

    if (!query.exec()) {
        qDebug() << "쿼리 실행 오류:" << query.lastError().text();
        return; // Handle query execution error
    }

    model->setQuery(query);

    if (model->rowCount() == 0) {
     QMessageBox::information(this, "No Messages Found", "사용자 '" + input + "'에 대한 메시지가 없습니다.");
    }

    // 열 제목 설정
    model->setHeaderData(0, Qt::Horizontal, "Name");
    model->setHeaderData(1, Qt::Horizontal, "ID");
    model->setHeaderData(2, Qt::Horizontal, "Message");
    model->setHeaderData(3, Qt::Horizontal, "Time");

    // 테이블 크기 조정
    ui->infoTable->resizeColumnsToContents();
}


 void DatabaseWindow::loadMessagesByKeyword() {
    QString keyword = QInputDialog::getText(this, "단어 입력", "검색할 단어를 입력하세요:");

    if (keyword.isEmpty()) return;
    model->clear();
    QSqlQuery query(dbManager->chatDatabase());
    query.prepare("SELECT name, userid, message, timestamp FROM chats WHERE message LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%"); // 매개변수 바인딩 (LIKE 조건을 위해 %로 감싸기)

    if (!query.exec()) {
        qDebug() << "쿼리 실행 오류:" << query.lastError().text();
        return;
    }

    // 쿼리 결과를 모델에 적용
    model->setQuery(query);

    if (model->rowCount() == 0) {
         QMessageBox::information(this, "검색 결과", "검색한 단어 '" + keyword + "'가 포함된 메시지가 없습니다.");
    }

    // 열 제목 설정
    model->setHeaderData(0, Qt::Horizontal, "Name");
    model->setHeaderData(1, Qt::Horizontal, "ID");
    model->setHeaderData(2, Qt::Horizontal, "Keyword");
    model->setHeaderData(3, Qt::Horizontal, "Time");

    // 테이블 크기 조정
    ui->infoTable->resizeColumnsToContents();
    ui->infoTable->setColumnWidth(0, 130);
    ui->infoTable->setColumnWidth(1, 100);
    ui->infoTable->setColumnWidth(2, 100);
    ui->infoTable->setColumnWidth(3, 150);
    ui->infoTable->resizeRowsToContents();
}

