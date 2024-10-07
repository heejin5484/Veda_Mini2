#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include <QDialog>
#include <QtSql/QSqlQueryModel>
#include "data.h"


namespace Ui {
class DataWindow;
}

class DataWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DataWindow(QWidget *parent = nullptr);
    ~DataWindow();
    void loadData(); // 데이터 로드 함수

private slots:
    void on_closeButton_clicked();  // Close 버튼 클릭 시 호출되는 슬롯
    void on_searchButton_clicked();  // Search 버튼 클릭 시 호출되는 슬롯
    void on_insertButton_clicked();  // Insert 버튼 클릭 시 호출되는 슬롯
    void on_updateButton_clicked();  // Update 버튼 클릭 시 호출되는 슬롯
    void on_deleteButton_clicked();  // Delete 버튼 클릭 시 호출되는 슬롯

private:
    Ui::DataWindow *ui;
    QSqlQueryModel *model;  // 데이터 모델
    //data dbManager;  // 데이터베이스 매니저 객체
};

#endif // DATAWINDOW_H
