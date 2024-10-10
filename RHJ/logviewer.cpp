#include "logviewer.h"
#include "ui_logviewer.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

logViewer::logViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logViewer),
    model(new QSqlQueryModel(this))
{
    ui->setupUi(this);
    setWindowTitle("Log Viewer");
    setFixedSize(600, 400);
    loadLogData();
}

logViewer::~logViewer()
{
    delete ui;
}

void logViewer::loadLogData()
{
    QSqlQuery query("SELECT timestamp, message FROM log_messages ORDER BY id ASC");
    if (!query.exec()) {
        qDebug() << "Error loading logs:" << query.lastError();
        return;
    }
    model->setQuery(query);
    ui->tableView->setModel(model);
}
