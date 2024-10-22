#include "chatroom.h"
#include "ui_chatroom.h"
#include "databasewindow.h"
#include <QMenu>
#include "rtpprocess.h"
#include <QCryptographicHash>
#include <QPainter>

QString generateRandomColorForUser(const QString& userId);
QIcon generateIconWithColor(const QColor& color);

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

    // 아이디에 맞는 색상 생성
    QColor userColor(generateRandomColorForUser(id));

    // 아이디에 맞는 색상으로 원을 그린 아이콘 생성
    QIcon icon = generateIconWithColor(userColor);

    // QListWidgetItem 생성 및 아이콘과 아이디를 함께 추가
    QListWidgetItem* item = new QListWidgetItem(icon, id);
    ui->userList->addItem(item);
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

QString generateRandomColorForUser(const QString& userId) {
    // 아이디를 해시 값으로 변환
    QByteArray hash = QCryptographicHash::hash(userId.toUtf8(), QCryptographicHash::Md5);

    // 해시 값을 기반으로 고유한 색상 생성 (RGB 값으로 변환)
    int r = static_cast<unsigned char>(hash[0]);
    int g = static_cast<unsigned char>(hash[1]);
    int b = static_cast<unsigned char>(hash[2]);

    // QColor 객체로 변환하여 색상 문자열로 반환
    QColor color(r, g, b);
    return color.name();  // #RRGGBB 형식의 색상 반환
}


// 사용자 아이디에 맞는 색상으로 원을 그린 아이콘을 생성하는 함수
QIcon generateIconWithColor(const QColor& color) {
    QPixmap pixmap(16, 16); // 아이콘 크기 설정
    pixmap.fill(Qt::transparent); // 투명 배경

    QPainter painter(&pixmap);
    painter.setBrush(QBrush(color)); // 색상을 브러쉬로 설정
    painter.setPen(Qt::NoPen); // 테두리 없음
    painter.drawEllipse(0, 0, 16, 16); // 원을 그림
    return QIcon(pixmap);
}


QString formatMessage(const QString& userId, const QString& message, const QString& userColor) {
    // 아이디에 따른 랜덤 색상 생성
    QString Color = generateRandomColorForUser(userId);

    // 현재 시간을 가져옴
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm");

    // userId와 message의 색상을 인자로 받아서 포맷, 그리고 시간 추가
    QString formattedMessage = QString("<b><font color='%1'>%2</font></b> :   <font color='black'>%3</font> "
                                       "<font color='#C0C0C0'>  [ %4 ]</font><br>")
                                   .arg(Color, userId, message, currentTime);
    return formattedMessage;
}

void chatRoom::onMessageReceived(const QString id, const QString &message)
{
    qDebug() << "emit 수신한 메시지:" << message; // 디버깅 출력
    QString msg = formatMessage(id, message, "red"); // test용
    // 수신된 HTML 메시지를 QTextEdit에 출력
    ui->chatLog->insertHtml(msg);
    ui->chatLog->moveCursor(QTextCursor::End); // 새로운 메시지가 올 때마다 끝으로 스크롤
}

