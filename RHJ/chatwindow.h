#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QQueue>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "thread.h"
#include "databasemanager.h"

namespace Ui {
class chatWindow;
}

class chatWindow : public QDialog
{
    Q_OBJECT

public:
    explicit chatWindow(QWidget *parent = 0);
    ~chatWindow();

private slots:
    void on_sendButton_clicked();
    void on_LogData_clicked();
    void queueUpdate();

    void on_pushButton_clicked();

    void on_dbData_clicked();

    void on_logData_clicked();

signals:
    void alert();

private:
    Ui::chatWindow *ui;
    QQueue<QString> msgqueue; // 메시지 큐
    int lastCopied = 0; // 마지막 복사된 인덱스
    databaseManager* dbManager;
    Thread *thread; // 스레드 인스턴스

    void saveMessage(const QString &message); // 메시지를 데이터베이스에 저장하는 메서드
    void loadLogData(); // 로그 데이터를 불러오는 메서드
};

#endif // CHATWINDOW_H
