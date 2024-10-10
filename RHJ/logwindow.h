#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include <QTextEdit>

namespace Ui {
class logWindow;
}

class logWindow : public QDialog
{
    Q_OBJECT

public:
    explicit logWindow(QWidget *parent = nullptr);
    ~logWindow();
    void setLogData(const QString &data);

private:
    Ui::logWindow *ui;
    QTextEdit *logTextEdit;
};

#endif // LOGWINDOW_H
