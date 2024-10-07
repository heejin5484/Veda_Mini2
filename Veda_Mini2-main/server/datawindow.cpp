#include "datawindow.h"
#include "ui_datawindow.h"

DataWindow::DataWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataWindow),
    model(new QSqlQueryModel(this))
{
    ui->setupUi(this);
    loadData();  // 데이터 로드
}

DataWindow::~DataWindow()
{
    delete ui;
}

void DataWindow::loadData(const QString &filter)
{
    dbManager.connectDB("C:\\Program Files\\SQLiteStudio\\newexample.db");  // DB 연결

    QString queryStr = "SELECT id, name, age FROM people";
    if (!filter.isEmpty()) {
        queryStr += " WHERE name LIKE '%" + filter + "%'";  // 필터 적용
    }

    QSqlQuery query(queryStr);
    model->setQuery(query);
    ui->tableView->setModel(model);  // 테이블 위젯에 모델 설정
}

void DataWindow::on_insertButton_clicked()
{
    QString name = ui->nameLineEdit->text();  // 이름 가져오기
    int age = ui->ageLineEdit->text().toInt();  // 나이 가져오기

    if (dbManager.insertData(name, age)) {
        loadData();  // 데이터 다시 로드
    }
}

void DataWindow::on_updateButton_clicked()
{
    // 선택된 행을 가져오고, 해당 ID로 업데이트 수행
    int selectedRow = ui->tableView->currentIndex().row();
    if (selectedRow >= 0) {
        int id = model->index(selectedRow, 0).data().toInt();  // 선택된 ID 가져오기
        QString name = ui->nameLineEdit->text();  // 이름 가져오기
        int age = ui->ageLineEdit->text().toInt();  // 나이 가져오기

        if (dbManager.updateData(id, name, age)) {
            loadData();  // 데이터 다시 로드
        }
    } else {
        qDebug() << "No row selected for update.";
    }
}

void DataWindow::on_deleteButton_clicked()
{
    // 선택된 행의 ID를 가져오고, 삭제 수행
    int selectedRow = ui->tableView->currentIndex().row();
    if (selectedRow >= 0) {
        int id = model->index(selectedRow, 0).data().toInt();  // 선택된 ID 가져오기

        if (dbManager.deleteData(id)) {
            loadData();  // 데이터 다시 로드
        }
    } else {
        qDebug() << "No row selected for deletion.";
    }
}
