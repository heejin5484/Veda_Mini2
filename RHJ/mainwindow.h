#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>
#include "thread.h"

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

private:
    Ui::MainWindow *ui;
    QQueue<QString> msgqueue;
    int lastCopied = 0;  // 마지막으로 복사된 인덱스
    Thread* thread;

};

#endif // MAINWINDOW_H
