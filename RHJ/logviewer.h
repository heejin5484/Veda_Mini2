#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class logViewer;
}

class logViewer : public QDialog
{
    Q_OBJECT

public:
    explicit logViewer(QWidget *parent = nullptr);
    ~logViewer();
    void loadLogData();

private:
    Ui::logViewer *ui;
    QSqlQueryModel *model;
};

#endif // LOGVIEWER_H
