#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>
#include "thread.h"
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sendButton_clicked();
    void queueUpdate();
    void on_LogData_clicked();
    void on_logDataButton_clicked();
    void on_pushButton_clicked();

    void on_idButton_1_clicked();

    void on_idButton_2_clicked();

    void on_idButton_3_clicked();

    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;
    QQueue<QString> msgqueue;
    int lastCopied = 0;  // 마지막으로 복사된 인덱스
    Thread* thread;
    databaseManager* dbManager;
};

#endif // MAINWINDOW_H
