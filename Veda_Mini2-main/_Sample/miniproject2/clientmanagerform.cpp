#include "clientmanagerform.h"
#include "ui_clientmanagerform.h"
#include "clientitem.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <QMenu>

ClientManagerForm::ClientManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManagerForm)
{
    ui->setupUi(this);

    QAction* removeAction = new QAction("&Remove");
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    ifstream file;
    file.open("clientlist.txt");
    if(!file.fail()) {
        while(!file.eof()) {
            vector<string> row = parseCSV(file, ',');
            if(row.size()) {
                int id = atoi(row[0].c_str());
                ClientItem* c = new ClientItem(id, row[1], row[2], row[3]);
                QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
                item->setText(row[0].c_str());
                ui->listWidget->setItemWidget(item, c);
                clientList.insert(id, c);
            }
        }
    }
    file.close( );
}

ClientManagerForm::~ClientManagerForm()
{
    delete ui;

    ofstream file;
    file.open("clientlist.txt");
    if(!file.fail()) {
        for (const auto& v : clientList) {
            ClientItem* c = v;
            file << c->id() << ", " << c->getName() << ", ";
            file << c->getPhoneNumber() << ", ";
            file << c->getAddress() << endl;
        }
    }
    file.close( );
}

void ClientManagerForm::inputClient( )
{
    string name, number, address;
    cout << "name : "; cin >> name;
    cout << "number : "; cin >> number;
    cout << "address : "; cin.ignore(); getline(cin, address, '\n'); //cin >> address;

    int id = makeId( );
    ClientItem* c = new ClientItem(id, name, number, address);
    clientList.insert(id, c);
}

ClientItem* ClientManagerForm::search(int id)
{
    return clientList[id];
}

void ClientManagerForm::deleteClient(int key)
{
    clientList.remove(key);
}

void ClientManagerForm::addClient(ClientItem* c)
{
    clientList.insert(c->id(), c);
}

int ClientManagerForm::makeId( )
{
    if(clientList.size( ) == 0) {
        return 0;
    } else {
        auto id = clientList.lastKey();
        return ++id;
    }
}

vector<string> ClientManagerForm::parseCSV(istream &file, char delimiter)
{
    stringstream ss;
    vector<string> row;
    string t = " \n\r\t";

    while(!file.eof()) {
        char c = file.get();
        if (c==delimiter || c=='\r' || c=='\n') {
            if(file.peek()=='\n') file.get();
            string s = ss.str();
            s.erase(0, s.find_first_not_of(t));
            s.erase(s.find_last_not_of(t)+1);
            row.push_back(s);
            ss.str("");
            if (c!=delimiter) break;
        } else {
            ss << c;
        }
    }
    return row;
}

void ClientManagerForm::on_pushButton_clicked()
{
    string name, number, address;
    int id = makeId( );
    name = ui->lineEditName->text().toUtf8();
    number = ui->lineEditPhoneNumber->text().toUtf8();
    address = ui->lineEditAddress->text().toUtf8();
    ClientItem* c = new ClientItem(id, name, number, address);
    clientList.insert(id, c);
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setText(QString::number(id));
    ui->listWidget->setItemWidget(item, c);
}

void ClientManagerForm::removeItem()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    clientList.remove(item->text().toInt());
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    ui->listWidget->update();
}

void ClientManagerForm::showContextMenu(const QPoint &pos)
{
    qDebug("ShowContextMenu");
    QPoint globalPos = ui->listWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void ClientManagerForm::on_pushButton_3_clicked()
{
    auto items = ui->listWidget->findItems(ui->lineEditSearch->text(), Qt::MatchCaseSensitive);

    ui->listWidgetSearch->clear();
    for (const auto& item : items) {
        ui->listWidgetSearch->addItem(item);
        QWidget *widget = ui->listWidget->itemWidget(item);
        QWidget* newWidget = new QWidget(this);

        QListWidgetItem* i = new QListWidgetItem(ui->listWidgetSearch);
        i->setText(item->text());

        int cnt = 0, xPos[3] = {40, 100, 200}, widths[3] = {60, 100, 200};
        if(widget != nullptr)
            for (const auto& c : widget->children()) {
                QLabel* l = qobject_cast<QLabel*>(c);
                QLabel* label = new QLabel(l->text(), newWidget);
                label->setGeometry(xPos[cnt], 0, widths[cnt], 18);
                cnt++;
                qDebug() << l->text();
            }
        ui->listWidgetSearch->setItemWidget(i, newWidget);
    }
}


void ClientManagerForm::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget->itemWidget(item);
    ui->lineEditID->setText(item->text());

    int cnt = 0;
    QList<QLineEdit*> leList;
    leList << ui->lineEditName;
    leList << ui->lineEditPhoneNumber;
    leList << ui->lineEditAddress;

    if(widget != nullptr)
        for (const auto& c : widget->children()) {
            QLabel* l = qobject_cast<QLabel*>(c);
            leList.at(cnt++)->setText(l->text());
        }
}

void ClientManagerForm::on_pushButtonModify_clicked()
{
    int key = ui->listWidget->currentItem()->text().toInt();
    ClientItem* c = search(key);
    string name, number, address;
    name = ui->lineEditName->text().toUtf8();
    number = ui->lineEditPhoneNumber->text().toUtf8();
    address = ui->lineEditAddress->text().toUtf8();
    c->setName(name);
    c->setPhoneNumber(number);
    c->setAddress(address);
    clientList.insert(key, c);
}

