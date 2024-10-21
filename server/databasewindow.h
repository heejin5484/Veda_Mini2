#ifndef DATABASEWINDOW_H
#define DATABASEWINDOW_H

#include <QDialog>
#include <QSqlQueryModel>
#include "databasemanager.h"

namespace Ui {
class DatabaseWindow;
}

class DatabaseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseWindow(QWidget *parent = nullptr);
    ~DatabaseWindow();

private slots:
    void onFilterChanged(const QString &filter);

private:
    Ui::DatabaseWindow *ui;  // UI 연결
    DatabaseManager *dbManager;  // DatabaseManager 포인터
    QSqlQueryModel *model;  // 테이블 데이터를 관리할 모델

    // 사용자 정보를 불러오는 함수
    void loadUsers();

    // 모든 메시지 정보를 불러오는 함수
    void loadMessages();

    // 특정 사용자의 메시지를 불러오는 함수
    void loadMessagesByUser();
};

#endif // DATABASEWINDOW_H
