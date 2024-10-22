#include "mainwindow.h"
#include "networkmanager.h"
#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkManager networkManager;
    loginWindow w(&networkManager);
    //MainWindow w;
    w.show();
    return a.exec();
}
