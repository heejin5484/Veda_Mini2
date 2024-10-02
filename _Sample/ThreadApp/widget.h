#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Thread;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private :
    Thread* thread;

public slots:
    void threadControl(bool flag);
};
#endif // WIDGET_H
