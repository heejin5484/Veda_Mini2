#include "searchwindow.h"
#include "ui_searchwindow.h"
#include "databasemanager.h"

SearchWindow::SearchWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
}

SearchWindow::~SearchWindow()
{
    delete ui;
}

void SearchWindow::on_SearchBtn_clicked()
{
    QString searchTerm = ui->searchLineEdt->text();
    QString logData;

    QSqlQuery query;

    query.prepare("SELECT timestamp, message FROM log_messages WHERE message LIKE :searchTerm");
    query.bindValue(":searchTerm", "%" + searchTerm + "%");

    if (query.exec()) {
        while (query.next()) {
            QString timestamp = query.value(0).toString();
            QString message = query.value(1).toString();
            logData += QString("[%1] %2\n").arg(timestamp, message);
        }
    } else {
        qDebug() << "데이터베이스에서 데이터 검색 실패: " << query.lastError().text();
        logData = "로그 데이터를 로드하는 데 실패했습니다.";
    }

    ui->resultTextEdit->setPlainText(logData);
}
