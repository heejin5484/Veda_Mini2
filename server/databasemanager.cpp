#include "databasemanager.h"
#include <QDir>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString &dbName) {
    // SQLite 드라이버 로드
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qDebug() << "SQLite 드라이버를 로드할 수 없습니다.";
        return;
    }

    QString connectionName = QString("connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

    // 절대 경로로 데이터베이스 파일 설정
    QString dbPath = dbName;
    db.setDatabaseName(dbPath);

    qDebug() << "Database path set to:" << db.databaseName();

    // 데이터베이스 파일이 없다면 생성
       if (!QFile::exists(dbPath)) {
           QFile file(dbPath);
           if (!file.open(QIODevice::ReadWrite)) {
               qDebug() << "Failed to create database file:" << file.errorString();
               return;
           }
           file.close();
       }

       // 데이터베이스 초기화
       if (!init()) {
           qDebug() << "Database initialization failed.";
       }
}

bool DatabaseManager::init() {
    // 데이터베이스 열기
    if (!db.open()) {
        qDebug() << "Database not opened:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database opened successfully";

    // 테이블 생성 쿼리
    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT, "
                    "phone TEXT, "
                    "email TEXT, "
                    "userid TEXT UNIQUE, "
                    "password TEXT, "
                    "type TEXT)")) {
        qDebug() << "Failed to create table: " << query.lastError().text();
        return false;
    }

    return true;
}


bool DatabaseManager::saveUserData(const QString& name, const QString& phone, const QString& email, const QString& userid, const QString& password, const QString& type)
{

    if (!db.isOpen()) {
            qDebug() << "Database is not open.";
            return false;
    }

    QSqlQuery query(db);  // 로컬 쿼리 생성
    query.prepare("INSERT INTO users (name, phone, email, userid, password, type) VALUES (?, ?, ?, ?, ?, ?)");

    // 바인딩 값 출력 및 바인딩
    qDebug() << "Binding values:" << name << phone << email << userid << password << type;

    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(email);
    query.addBindValue(userid);
    query.addBindValue(password);
    query.addBindValue(type);

    // 쿼리 실행
    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::loadUsers() {
    QSqlQuery query(db);
    if (!query.exec("SELECT name, phone, email, userid, password FROM users")) {
        qDebug() << "Query execution error:" << query.lastError().text();  // 오류 메시지 출력
    }
    return query;  // 쿼리 결과 반환
}


// 메시지 데이터를 로드하는 함수
QSqlQuery DatabaseManager::loadMessages() {
    QSqlQuery query(db);
    if (!query.exec("SELECT messages.message, users.userid, messages.timestamp FROM messages JOIN users ON messages.user_id = users.id")) {
        qDebug() << "Message loading error:" << query.lastError().text();  // 오류 메시지 출력
    }
    return query;  // 쿼리 결과 반환
}

// 추가된 멤버 함수
QSqlDatabase DatabaseManager::database() const {
    return db;  // 데이터베이스 객체를 반환
}

DatabaseManager::~DatabaseManager() {
    close(); // 데이터베이스 닫기
}

void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
    //QSqlDatabase::removeDatabase(db.connectionName()); // 연결 이름을 사용하여 제거
}
