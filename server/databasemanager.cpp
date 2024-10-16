#include "databasemanager.h"
#include <QDir>
#include <QThread>

DatabaseManager::DatabaseManager(const QString &dbName) {
    // SQLite 드라이버 로드
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qDebug() << "SQLite 드라이버를 로드할 수 없습니다.";
        return;
    }

    QString connectionName = QString("connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(QDir::currentPath() + "/" + dbName); // 파일 경로를 설정
    qDebug() << "Database path set to:" << db.databaseName();
}

bool DatabaseManager::init() {
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database opened successfully";

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


bool DatabaseManager::saveUserData(const QString& name, const QString& phone, const QString& email, const QString& userid, const QString& password, const QString& type) {
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
    query.exec("SELECT messages.message, users.userid, messages.timestamp FROM messages JOIN users ON messages.user_id = users.id");
    return query;  // 쿼리 결과 반환
}


DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(db.connectionName());
}
