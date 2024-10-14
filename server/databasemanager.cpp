#include "databasemanager.h"

DatabaseManager::DatabaseManager(const QString& dbName)
    : databaseName(dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseName);
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

// DB 테이블 생성 및 초기화
bool DatabaseManager::init()
{
    if (!db.open()) {
        qDebug() << "Database connection failed: " << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS users ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "username TEXT UNIQUE, "
                          "password TEXT, "
                          "name TEXT, "
                          "phone TEXT, "
                          "email TEXT)";

    if (!query.exec(createTable)) {
        qDebug() << "Failed to create table: " << query.lastError().text();
        return false;
    }

    return true;
}

// 사용자 정보를 데이터베이스에 저장
bool DatabaseManager::saveUserData(const QString &username, const QString &password, const QString &name, const QString &phone, const QString &email) {
    // 사용자 이름이 이미 존재하는지 확인
    if (checkUser(username, password)) {
        qDebug() << "Username already exists: " << username;
        return false; // 이미 존재하는 사용자
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, name, phone, email) VALUES (:username, :password, :name, :phone, :email)");
    query.bindValue(":username", username);
    query.bindValue(":password", password); // 비밀번호는 해시 처리하는 것이 좋음
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Failed to save user data to database: " << query.lastError().text();
        return false;
    }
    return true;
}

// 사용자 존재 여부 및 비밀번호 검증
bool DatabaseManager::checkUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Database query error: " << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString storedPassword = query.value(0).toString();
        // 비밀번호 해시 검증 로직 추가 필요
        return storedPassword == password; // 단순 비교, 실제로는 해시 비교 사용
    }

    return false; // 사용자 없음
}
