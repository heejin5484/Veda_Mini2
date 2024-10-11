#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(new ChatServer(this))
{
    ui->setupUi(this);
    startServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startServer() {
    if (!server->listen(QHostAddress::Any, 8888)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port" << server->serverPort();
    }
}
