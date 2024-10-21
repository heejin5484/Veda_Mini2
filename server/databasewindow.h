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
    void refreshData();

private:
    Ui::DatabaseWindow *ui;  // UI 연결
    DatabaseManager *dbManager;  // DatabaseManager 포인터
    QSqlQueryModel *model;  // 테이블 데이터를 관리할 모델

    void loadUsers();
    void loadMessages();
    void loadMessagesByUser();
    void loadMessagesByKeyword();
};

#endif // DATABASEWINDOW_H
