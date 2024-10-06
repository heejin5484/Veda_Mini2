#ifndef MAINWINDOW_CHAT_H
#define MAINWINDOW_CHAT_H

#include <QMainWindow>
#include "mainwindow_chat.h"  // ui_mainwindow_chat.h를 포함

namespace Ui {
class MainWindowChat;
}

class MainWindowChat : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowChat(QWidget *parent = nullptr);
    ~MainWindowChat();

private:
    Ui::MainWindowChat *ui;  // UI를 위한 멤버 변수
};



#endif // MAINWINDOW_CHAT_H
