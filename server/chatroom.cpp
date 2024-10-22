#include "chatroom.h"
#include "ui_chatroom.h"
#include "databasewindow.h"
#include <QMenu>
#include "rtpprocess.h"
chatRoom::chatRoom(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatRoom)
{
    ui->setupUi(this);
    ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);
    //chatServer = new ChatServer(this);
    //connect(chatServer, &ChatServer::messageReceived, this, &chatRoom::onMessageReceived);
    qDebug() << "Signal connected!";
    connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(rtpProcess::instance(), &rtpProcess::newFrameAvailable, this, &chatRoom::onNewFrameAvailable);
}

chatRoom::~chatRoom()
{
    delete ui;
}

void chatRoom::onNewFrameAvailable(const QImage& frame) {
    ui->video_label->setPixmap(QPixmap::fromImage(frame));
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

QString formatMessage(const QString& userId, const QString& message) {
    // userId를 파란색으로, 메시지를 검정색으로 서식 지정
    QString formattedMessage = QString("<b><font color='blue'>%1</font></b>: <font color='black'>%2</font><br>")
                                   .arg(userId, message);
    return formattedMessage;
}

QString formatMessage(const QString& userId, const QString& message, const QString& userColor) {
    // userId와 message의 색상을 인자로 받아서 포맷
    QString formattedMessage = QString("<b><font color='%1'>%2</font></b>: <font color='black'>%3</font><br>")
                                   .arg(userColor, userId, message);
    return formattedMessage;
}

void chatRoom::onMessageReceived(const QString &message)
{
    qDebug() << "emit 수신한 메시지:" << message; // 디버깅 출력
    QString msg = formatMessage("heejin", message, "red");
    // 수신된 HTML 메시지를 QTextEdit에 출력
    ui->chatLog->insertHtml(msg);
    ui->chatLog->moveCursor(QTextCursor::End); // 새로운 메시지가 올 때마다 끝으로 스크롤
}

