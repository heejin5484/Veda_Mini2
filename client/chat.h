#ifndef CHAT_H
#define CHAT_H

#include <QWidget>

namespace Ui {
class chat;
}

class chat : public QWidget
{
    Q_OBJECT

public:
    explicit chat(QWidget *parent = nullptr);
    ~chat();

private:
    Ui::chat *ui;

signals :
    void sendMsg_sig(const QString &msg);

private slots:
    void onSendButtonClicked();
};

#endif // CHAT_H
