#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>

namespace Ui {
class chatRoom;
}

class chatRoom : public QWidget
{
    Q_OBJECT

public:
    explicit chatRoom(QWidget *parent = nullptr);
    ~chatRoom();

public slots:
    void showContextMenu(const QPoint &pos);

private:
    Ui::chatRoom *ui;
};

#endif // CHATROOM_H
