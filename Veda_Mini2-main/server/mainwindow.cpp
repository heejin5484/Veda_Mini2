#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataWindow = new DataWindow(this);  // 데이터 윈도우 객체 생성


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_showDataButton_clicked()
{
    dataWindow->show();  // 데이터 윈도우 열기
}
