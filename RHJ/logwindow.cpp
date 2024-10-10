#include "logwindow.h"
#include "ui_logwindow.h"

logWindow::logWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logWindow)
{
    ui->setupUi(this);
    setWindowTitle("Log Data");
    setFixedSize(600, 400);
}

logWindow::~logWindow()
{
    delete ui;
}

void logWindow::setLogData(const QString &data)
{
    ui->logTextEdit->setPlainText(data);
}
