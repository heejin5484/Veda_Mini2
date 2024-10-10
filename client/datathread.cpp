#include "datathread.h"
#include <QDebug>

datathread::datathread(QObject *obj) : QThread(obj){}

void datathread::run()
{
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &datathread::onTimeout);

    timer->start(10000); // 10초마다 timeout
    exec();
}

void datathread::onTimeout(){
    qDebug() << "aa";
    emit alert();
}
