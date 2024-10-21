#include "databasemanager.h"
#include <QDir>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString &userDbName, const QString &chatDbName) {
    // SQLite 드라이버 확인
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qDebug() << "SQLite 드라이버를 로드할 수 없습니다.";
        return;
    }

    // 각각의 스레드에 맞는 연결 이름 설정
    QString userConnectionName = QString("user_connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
    QString chatConnectionName = QString("chat_connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));

    // 유저 DB 설정
    if (QSqlDatabase::contains(userConnectionName)) {
        userDb = QSqlDatabase::database(userConnectionName);
    } else {
        userDb = QSqlDatabase::addDatabase("QSQLITE", userConnectionName);
    }
    userDb.setDatabaseName(userDbName);

    // 채팅 DB 설정
    if (QSqlDatabase::contains(chatConnectionName)) {
        chatDb = QSqlDatabase::database(chatConnectionName);
    } else {
        chatDb = QSqlDatabase::addDatabase("QSQLITE", chatConnectionName);
    }
    chatDb.setDatabaseName(chatDbName);

    qDebug() << "User DB path set to:" << userDb.databaseName();
    qDebug() << "Chat DB path set to:" << chatDb.databaseName();

    // 데이터베이스 파일이 없다면 생성
    if (!QFile::exists(userDbName)) {
        QFile file(userDbName);
        if (!file.open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to create user database file:" << file.errorString();
            return;
        }
        file.close();
    }

    if (!QFile::exists(chatDbName)) {
        QFile file(chatDbName);
        if (!file.open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to create chat database file:" << file.errorString();
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
    // 유저 테이블 생성
    if (!userDb.open()) {
        qDebug() << "User database not opened:" << userDb.lastError().text();
        return false;
    }

    QSqlQuery userQuery(userDb);
    if (!userQuery.exec("CREATE TABLE IF NOT EXISTS users ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "name TEXT, "
                        "phone TEXT, "
                        "email TEXT, "
                        "userid TEXT UNIQUE, "
                        "password TEXT, "
                        "type TEXT)")) {
        qDebug() << "Failed to create users table: " << userQuery.lastError().text();
        return false;
    }

    // 채팅 테이블 생성
    if (!chatDb.open()) {
        qDebug() << "Chat database not opened:" << chatDb.lastError().text();
        return false;
    }

    QSqlQuery chatQuery(chatDb);
    if (!chatQuery.exec("CREATE TABLE IF NOT EXISTS chats ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "userid TEXT, "
                        "message TEXT, "
                        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Failed to create log_messages table: " << chatQuery.lastError().text();
        return false;
    }

    return true;
}

// 유저 데이터 저장 함수
bool DatabaseManager::saveUserData(const QString& name, const QString& phone, const QString& email, const QString& userid, const QString& password, const QString& type) {
    if (!userDb.isOpen()) {
        qDebug() << "User database is not open.";
        return false;
    }

    QSqlQuery query(userDb);
    query.prepare("INSERT INTO users (name, phone, email, userid, password, type) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(email);
    query.addBindValue(userid);
    query.addBindValue(password);
    query.addBindValue(type);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        return false;
    }
    return true;
}


bool DatabaseManager::saveMessage(const QString& userid, const QString &message) {
    if (!chatDb.isOpen()) {
        qDebug() << "Chat database not open.";
        return false;
    }

    QSqlQuery query(chatDb);
    query.prepare("INSERT INTO chats (userid, message, timestamp) VALUES (?, ?, CURRENT_TIMESTAMP)");
    query.addBindValue(userid);
    query.addBindValue(message);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        return false;
    }
    return true;
}


// 유저 데이터 로드 함수
QSqlQuery DatabaseManager::loadUsers() {
    QSqlQuery query(userDb);
    if (!query.exec("SELECT name, phone, email, userid, password FROM users")) {
        qDebug() << "User data loading error:" << query.lastError().text();
    }
    return query;
}

// 채팅 메시지 로드 함수
QSqlQuery DatabaseManager::loadMessages() {
    QSqlQuery query(chatDb);
    if (!query.exec("SELECT message, userid, timestamp FROM chats")) {
        qDebug() << "Message loading error:" << query.lastError().text();
    }
    return query;
}

void DatabaseManager::close() {
    if (userDb.isOpen()) {
        userDb.close();
        qDebug() << "User database connection closed.";
    }
    if (chatDb.isOpen()) {
        chatDb.close();
        qDebug() << "Chat database connection closed.";
    }
}

DatabaseManager::~DatabaseManager() {
    close();
}



QSqlDatabase DatabaseManager::database() const {
    return userDb;  // 데이터베이스 객체를 반환
}
