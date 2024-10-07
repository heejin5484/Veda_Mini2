#ifndef DATA_H
#define DATA_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>  // 추가
#include <QDebug>
#include <QVariant>

class data
{
public:
    data();
    ~data();

    bool connectDB(const QString &path);
    bool createTable();
    bool insertData(const QString &name, int age);
    bool updateData(int id, const QString &name, int age);  // 데이터 업데이트
    bool deleteData(int id);  // 데이터 삭제
    QSqlQueryModel* selectData();  // 데이터 조회 함수

private:
    QSqlDatabase db;
};

#endif // DATA_H
