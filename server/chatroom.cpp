#include "chatroom.h"
#include "ui_chatroom.h"
#include "databasewindow.h"
#include <QMenu>

chatRoom::chatRoom(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatRoom)
{
    ui->setupUi(this);

    ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}


chatRoom::~chatRoom()
{
    delete ui;
}


void chatRoom::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = ui->userList->itemAt(pos);
    if (item) {
        QMenu contextMenu;
        QAction *privateChat = contextMenu.addAction("1:1 채팅");
        QAction *kickUser = contextMenu.addAction("강퇴");

        connect(privateChat, &QAction::triggered, this, [item]() {
            // 1:1 채팅 기능
            qDebug() << "Starting 1:1 chat with" << item->text();
        });

        connect(kickUser, &QAction::triggered, this, [item]() {
            // 강퇴 기능
            qDebug() << "Kicking" << item->text();
        });

        contextMenu.exec(ui->userList->mapToGlobal(pos));
    }
}

void chatRoom::addUserList(QString id){
    qDebug() << id;
    ui->userList->addItem(id);
}

void chatRoom::deleteUserList(QString id){
    qDebug() << id << " disconnected.";

    // QListWidget에서 해당 ID를 찾음
    QList<QListWidgetItem*> items = ui->userList->findItems(id, Qt::MatchExactly);

    // 찾은 항목이 있을 경우 제거
    if (!items.isEmpty()) {
        // 첫 번째 항목을 가져와서 제거
        delete ui->userList->takeItem(ui->userList->row(items[0]));
        qDebug() << id << " removed from user list.";
    } else {
        qDebug() << id << " not found in user list.";
    }
}

void chatRoom::on_managerButton_clicked()
{
    DatabaseWindow *dbWindow = new DatabaseWindow(this);
    dbWindow->exec();

}

