#ifndef CLIENTMANAGERFORM_H
#define CLIENTMANAGERFORM_H

#include <QWidget>
#include <QHash>

#include "clientitem.h"

class QMenu;
class QListWidgetItem;

namespace Ui {
class ClientManagerForm;
}

class ClientManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManagerForm(QWidget *parent = nullptr);
    ~ClientManagerForm();

    void inputClient( );
    void addClient(ClientItem*);
    void deleteClient(int);
    void modifyClient(int);
    ClientItem* search(int);
    int makeId();
    vector<string> parseCSV(istream&, char);

private slots:
    void on_pushButton_clicked();
    void removeItem();
    void showContextMenu(const QPoint &);

    void on_pushButton_3_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButtonModify_clicked();

private:
    QMap<int, ClientItem*> clientList;
    Ui::ClientManagerForm *ui;
    QMenu* menu;
};

#endif // CLIENTMANAGERFORM_H
